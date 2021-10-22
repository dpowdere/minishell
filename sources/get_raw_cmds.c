/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_raw_cmds.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 18:38:40 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/21 15:43:28 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define SPACE_CHAR	' '

static enum e_operator	get_operator_type(const char *line)
{
	if (!ft_memcmp(line, "||", 2))
		return (OPERATOR_OR);
	else if (!ft_memcmp(line, "&&", 2))
		return (OPERATOR_AND);
	else if (!ft_memcmp(line, "<<", 2))
		return (OPERATOR_REDIRECT_IN_STOPWORD);
	else if (!ft_memcmp(line, ">>", 2))
		return (OPERATOR_REDIRECT_OUT_APPEND);
	else if (*line == '|')
		return (OPERATOR_PIPE);
	else if (*line == '<')
		return (OPERATOR_REDIRECT_IN);
	else if (*line == '>')
		return (OPERATOR_REDIRECT_OUT);
	else if (*line == '(')
		return (OPERATOR_SUBSHELL_IN);
	else if (*line == ')')
		return (OPERATOR_SUBSHELL_OUT);
	else
		return (OPERATOR_NONE);
}

static size_t	subshell_arg_strlen(t_list *start, t_list *end)
{
	size_t	len;

	len = 0;
	while (start != end)
	{
		len += ft_strlen(((t_token *)start->content)->string) + 1;
		start = start->next;
	}
	return (len);
}

static char	*populate_subshell_arg(char *arg, t_list *start, t_list *end)
{
	char	*cursor;
	t_token	*token;
	size_t	len;

	cursor = arg;
	while (start != end)
	{
		token = (t_token *)start->content;
		len = ft_strlen(token->string);
		ft_strlcpy(cursor, token->string, len + 1);
		cursor += len;
		*cursor++ = SPACE_CHAR;
		start = start->next;
	}
	if (cursor != arg)
		--cursor;
	*cursor = '\0';
	return (arg);
}

static bool	got_arg_for_subshell(t_cmd *cmd, t_list *start, t_list *end)
{
	char	*arg;

	arg = malloc(subshell_arg_strlen(start, end));
	if (arg == NULL)
		return (false);
	cmd->args_list = ft_lstnew(populate_subshell_arg(arg, start, end));
	if (cmd->args_list == NULL)
	{
		free(arg);
		return (false);
	}
	return (true);
}

static t_cmd	*get_subshell_cmd(t_list **global_cursor)
{
	t_cmd	*cmd;
	t_list	*local_cursor;
	t_list	*start;
	t_list	*end;
	t_token	*token;
	int		brackets_to_close;

	cmd = ft_calloc(1, sizeof(t_cmd));
	if (cmd == NULL)
		return (NULL);
	brackets_to_close = 1;
	start = (*global_cursor)->next;
	end = start;
	local_cursor = start;
	while (brackets_to_close && local_cursor)
	{
		token = (t_token *)local_cursor->content;
		end = local_cursor;
		if (token->type == TOKEN_OPERATOR
			&& get_operator_type(token->string) == OPERATOR_SUBSHELL_IN)
			++brackets_to_close;
		else if (token->type == TOKEN_OPERATOR
			&& get_operator_type(token->string) == OPERATOR_SUBSHELL_OUT)
			--brackets_to_close;
		local_cursor = local_cursor->next;
	}
	cmd->cmd = ft_strdup("minishell");
	if (cmd->cmd == NULL)
	{
		free(cmd);
		return (error(ERR_ERRNO, NULL, NULL, NULL));
	}
	if (got_arg_for_subshell(cmd, start, end) == false)
	{
		free_cmd(cmd);
		return (error(ERR_ERRNO, NULL, NULL, NULL));
	}
	if (local_cursor)
	{
		token = (t_token *)local_cursor->content;
		if (token->type == TOKEN_OPERATOR)
			cmd->next_operator = get_operator_type(token->string);
		else
		{
			free_cmd(cmd);
			return (error(ERR_SYNTAX_TOKEN, ERR_STR_SYNTAX_TOKEN, NULL, NULL));
		}
		*global_cursor = local_cursor->next;
	}
	else
	{
		cmd->next_operator = OPERATOR_NONE;
		*global_cursor = local_cursor;
	}
	return (cmd);
}

static inline bool	is_redirect_operator(t_token *token)
{
	enum e_operator	op;

	if (token->type != TOKEN_OPERATOR)
		return (false);
	op = get_operator_type(token->string);
	return (op == OPERATOR_REDIRECT_IN || op == OPERATOR_REDIRECT_IN_STOPWORD
		|| op == OPERATOR_REDIRECT_OUT || op == OPERATOR_REDIRECT_OUT_APPEND);
}

static inline bool	is_non_redirect_operator(t_token *token)
{
	enum e_operator	op;

	if (token->type != TOKEN_OPERATOR)
		return (false);
	op = get_operator_type(token->string);
	return (op != OPERATOR_REDIRECT_IN && op != OPERATOR_REDIRECT_IN_STOPWORD
		&& op != OPERATOR_REDIRECT_OUT && op != OPERATOR_REDIRECT_OUT_APPEND);
}

static t_list	*populate_cmd(t_cmd **cmd, t_list *cursor)
{
	(*cmd)->cmd = ft_strdup(((t_token *)cursor->content)->string);
	if ((*cmd)->cmd == NULL)
	{
		free_cmd(*cmd);
		*cmd = NULL;
	}
	return (cursor->next);
}

static t_list	*populate_arg(t_cmd **cmd, t_list *cursor)
{
	char	*arg;
	t_list	*item;

	arg = ft_strdup(((t_token *)cursor->content)->string);
	if (arg == NULL)
	{
		free_cmd(*cmd);
		*cmd = NULL;
		return (NULL);
	}
	item = ft_lstnew(arg);
	if (item == NULL)
	{
		free(arg);
		free_cmd(*cmd);
		*cmd = NULL;
		return (NULL);
	}
	ft_lstadd_back(&(*cmd)->args_list, item);
	return (cursor->next);
}

static t_list	*populate_redirect(t_cmd **cmd, t_list *cursor)
{
	char		*word;
	t_redirect	*redir;
	t_list		*item;

	redir = ft_calloc(1, sizeof(t_redirect));
	if (redir == NULL)
	{
		free_cmd(*cmd);
		*cmd = NULL;
		return (NULL);
	}
	redir->type = get_operator_type(((t_token *)cursor->content)->string);
	cursor = cursor->next;
	if (cursor == NULL)
	{
		free(redir);
		free_cmd(*cmd);
		*cmd = NULL;
		return (NULL);
	}
	word = ft_strdup(((t_token *)cursor->content)->string);
	if (word == NULL)
	{
		free(redir);
		free_cmd(*cmd);
		*cmd = NULL;
		return (NULL);
	}
	redir->word = word;
	item = ft_lstnew(redir);
	if (item == NULL)
	{
		free(word);
		free(redir);
		free_cmd(*cmd);
		*cmd = NULL;
		return (NULL);
	}
	if (redir->type == OPERATOR_REDIRECT_IN
		|| redir->type == OPERATOR_REDIRECT_IN_STOPWORD)
		ft_lstadd_back(&(*cmd)->redirect_in, item);
	else if (redir->type == OPERATOR_REDIRECT_OUT
		|| redir->type == OPERATOR_REDIRECT_OUT_APPEND)
		ft_lstadd_back(&(*cmd)->redirect_out, item);
	return (cursor->next);
}

static t_cmd	*get_cmd(t_list **global_cursor)
{
	t_cmd	*cmd;
	t_list	*local_cursor;
	t_token	*token;

	cmd = ft_calloc(1, sizeof(t_cmd));
	if (cmd == NULL)
		return (NULL);
	local_cursor = *global_cursor;
	while (local_cursor && cmd)
	{
		token = (t_token *)local_cursor->content;
		if (is_non_redirect_operator(token))
		{
			cmd->next_operator = get_operator_type(token->string);
			local_cursor = local_cursor->next;
			break ;
		}
		else if (is_redirect_operator(token))
			local_cursor = populate_redirect(&cmd, local_cursor);
		else if (cmd->cmd == NULL)
			local_cursor = populate_cmd(&cmd, local_cursor);
		else
			local_cursor = populate_arg(&cmd, local_cursor);
	}
	*global_cursor = local_cursor;
	return (cmd);
}

t_list	*get_cmds_list(t_list *cursor)
{
	t_list	*cmd_list;
	t_list	*new_item;
	t_token	*token;
	t_cmd	*cmd;

	cmd_list = NULL;
	while (cursor)
	{
		token = (t_token *)cursor->content;
		if (token->type == TOKEN_OPERATOR
			&& get_operator_type(token->string) == OPERATOR_SUBSHELL_IN)
			cmd = get_subshell_cmd(&cursor);
		else
			cmd = get_cmd(&cursor);
		if (cmd == NULL)
			return (error(ERR_ERRNO, NULL, cmd_list, free_cmd));
		new_item = ft_lstnew(cmd);
		if (new_item == NULL)
		{
			free_cmd(cmd);
			return (error(ERR_ERRNO, NULL, cmd_list, free_cmd));
		}
		ft_lstadd_front(&cmd_list, new_item);
	}
	return (debug_raw_cmds(ft_lstreverse(&cmd_list)));
}
