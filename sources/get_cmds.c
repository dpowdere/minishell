/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_cmds.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 18:38:40 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/11 18:43:15 by ngragas          ###   ########.fr       */
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

static void	remove_escape_char(char *line, bool opened_double_quote)
{
	const char	line_char = *(line + 1);

	if (opened_double_quote == false)
		ft_memmove(line, line + 1, ft_strlen(line));
	else if (line_char == '\"' || line_char == '\\')
		ft_memmove(line, line + 1, ft_strlen(line));
}

static char	*skip_brackets_inside(char *line)
{
	t_opened	opened;

	opened = (t_opened){0};
	opened.brackets_count = 1;
	while (*line && opened.brackets_count)
	{
		if (*line == ')' && !opened.single_quote && !opened.double_quote)
			opened.brackets_count--;
		if (opened.brackets_count && !ft_isspace(*line))
			opened.words_started = true;
		if (*line == '\\' && *(line + 1) != '\0' && !opened.single_quote)
			line++;
		else if (*line == '\'' && !opened.double_quote)
			opened.single_quote = !opened.single_quote;
		else if (*line == '\"' && !opened.single_quote)
			opened.double_quote = !opened.double_quote;
		else if (*line == '(' && !opened.single_quote && !opened.double_quote)
			opened.brackets_count++;
		line++;
	}
	if (opened.words_started == false || opened.brackets_count)
		return (NULL);
	else
		return (line);
}

static char	*parse_skip_brackets(char *line, bool words_started)
{
	if (words_started)
		return (NULL);
	line = skip_brackets_inside(line + 1);
	if (line == NULL)
		return (NULL);
	while (ft_isspace(*line))
		line++;
	if (*line == '\0' || *line == '|' || (*line == '&' && *(line + 1) == '&') \
		|| *line == '<' || *line == '>')
		return (line);
	else
		return (NULL);
}

static char	*remove_quote(char *line, bool *opened_quote)
{
	ft_memmove(line, line + 1, ft_strlen(line));
	*opened_quote = !*opened_quote;
	return (line - 1);
}

static char	*parse_find_cmd_end(char *line)
{
	t_opened	opened;

	opened = (t_opened){0};
	while (*line && !((line[0] == '|' || (line[0] == '&' && line[1] == '&')) \
				&& !opened.single_quote && !opened.double_quote))
	{
		if (!ft_isspace(*line))
			opened.words_started = true;
		if (*line == '\'' && !opened.double_quote)
			line = remove_quote(line, &opened.single_quote);
		else if (*line == '\"' && !opened.single_quote)
			line = remove_quote(line, &opened.double_quote);
		else if (*line == '\\' && !opened.single_quote)
			remove_escape_char(line, opened.double_quote);
		else if (*line == ')' && !opened.single_quote && !opened.double_quote)
			return (NULL);
		else if (*line == '(' && !opened.single_quote && !opened.double_quote)
			line = parse_skip_brackets(line, opened.words_started);
		if (line == NULL)
			return (NULL);
		line++;
	}
	if (!opened.words_started || opened.single_quote || opened.double_quote)
		return (NULL);
	return (line);
}

t_list	*get_cmds(t_list *words_list)
{
	t_list	*word_list;
	char	*cmd_end;
	t_list	*cmd_list;
	t_list	*new_list;
	t_cmd	*cmd;

	word_list = get_words(line);

	cmd_list = NULL;
	while (*line)
	{
		cmd_end = parse_find_cmd_end(line);
		if (cmd_end == NULL)
			return (error(ERR_SYNTAX, ERR_SYNTAX_CODE, cmd_list));
		new_list = ft_lstnew(ft_calloc(1, sizeof(*cmd)));
		if (new_list == NULL || new_list->content == NULL)
			return (error(strerror(errno), errno, cmd_list));
		cmd = new_list->content;
		cmd->cmd = ft_substr(line, 0, cmd_end - line);
		if (cmd->cmd == NULL)
			return (error(strerror(errno), errno, cmd_list));
		line = parse_get_operator(cmd_end, &cmd->next_operator);
		if (*line == '\0' && cmd->next_operator != OPERATOR_NONE)
			return (error(ERR_SYNTAX, ERR_SYNTAX_CODE, cmd_list));
		ft_lstadd_back(&cmd_list, new_list);
	}
	return (cmd_list);
}
