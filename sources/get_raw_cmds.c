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

static t_list	*popconvert_tokenlst_to_stringlst(t_list **tokens_list)
{
	t_list	*stringlst;
	t_token	*token;

	stringlst = *tokens_list;
	token = (*tokens_list)->content;
	*tokens_list = (*tokens_list)->next;
	stringlst->content = token->string;
	stringlst->next = NULL;
	free(token);
	return (stringlst);
}

static t_list	*get_cmd_subshell(t_list *tokens_list, t_cmd *cmd)
{
	int		brackets_to_close;
	t_token	*token;

	brackets_to_close = 1;
	token = tokens_list->content;
	free(token->string);
	token->string = ft_strdup(COMMAND_NAME);
	if (token->string == NULL)
		return (tokens_list);
	while (brackets_to_close > 0)
	{
		ft_lstadd_back(&cmd->args_list,
			popconvert_tokenlst_to_stringlst(&tokens_list));
		token = tokens_list->content;
		if (token->type == TOKEN_OPERATOR)
		{
			if (get_operator_type(token->string) == OPERATOR_SUBSHELL_IN)
				++brackets_to_close;
			else if (get_operator_type(token->string) == OPERATOR_SUBSHELL_OUT)
				--brackets_to_close;
		}
	}
	free_token(ft_lstpop(&tokens_list));
	return (tokens_list);
}

static t_list	*get_operator_redirect(t_list *tokens_list, t_cmd *cmd,
										enum e_operator operator)
{
	t_redirect	*redirect;
	t_list		*redirectlst;

	redirect = malloc(sizeof(*redirect));
	if (redirect == NULL)
		return (tokens_list);
	redirectlst = popconvert_tokenlst_to_stringlst(&tokens_list);
	free(redirectlst->content);
	redirectlst->content = redirect;
	redirectlst->next = NULL;
	redirect->type = operator;
	redirect->target = ((t_token *)tokens_list->content)->string;
	free(ft_lstpop(&tokens_list));
	if (operator == OPERATOR_REDIRECT_IN
		|| operator == OPERATOR_REDIRECT_IN_STOPWORD)
		ft_lstadd_back(&cmd->redirect_in, redirectlst);
	else if (operator == OPERATOR_REDIRECT_OUT
		|| operator == OPERATOR_REDIRECT_OUT_APPEND)
		ft_lstadd_back(&cmd->redirect_out, redirectlst);
	return (tokens_list);
}

static t_list	*get_operator(t_list *tokens_list, t_cmd *cmd,
							enum e_operator operator)
{
	if (operator == OPERATOR_PIPE
		|| operator == OPERATOR_OR
		|| operator == OPERATOR_AND)
	{
		cmd->next_operator = operator;
		free_token(ft_lstpop(&tokens_list));
	}
	else
		tokens_list = get_operator_redirect(tokens_list, cmd, operator);
	return (tokens_list);
}

t_list	*get_cmd(t_list *tokens_list, t_cmd *cmd)
{
	t_token			*token;
	enum e_operator	operator;

	operator = OPERATOR_NONE;
	while (tokens_list && errno == 0 && operator != OPERATOR_PIPE && \
		operator != OPERATOR_OR && operator != OPERATOR_AND)
	{
		token = tokens_list->content;
		if (token->type == TOKEN_WORD)
			ft_lstadd_back(&cmd->args_list,
				popconvert_tokenlst_to_stringlst(&tokens_list));
		else if (token->type == TOKEN_OPERATOR)
		{
			operator = get_operator_type(token->string);
			if (operator == OPERATOR_SUBSHELL_IN)
				tokens_list = get_cmd_subshell(tokens_list, cmd);
			else
				tokens_list = get_operator(tokens_list, cmd, operator);
		}
	}
	return (tokens_list);
}

t_list	*get_cmds_list(t_list *tokens_list)
{
	t_cmd	*cmd;
	t_list	*cmdlst;
	t_list	*cmds_list;

	cmds_list = NULL;
	while (tokens_list && errno == 0)
	{
		cmd = ft_calloc(1, sizeof(*cmd));
		cmdlst = ft_lstnew(cmd);
		if (cmd == NULL || cmdlst == NULL)
		{
			free(cmd);
			break ;
		}
		ft_lstadd_front(&cmds_list, cmdlst);
		tokens_list = get_cmd(tokens_list, cmd);
	}
	if (errno)
	{
		ft_lstclear(&tokens_list, free_token);
		return (error(ERR_ERRNO, NULL, cmds_list, free_cmd));
	}
	return (debug_raw_cmds(ft_lstreverse(&cmds_list)));
}
