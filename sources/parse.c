/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 18:38:40 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/11 18:43:15 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	*error(char *message, int error_code, t_list *cmd_list_to_free)
{
	ft_lstclear(&cmd_list_to_free, free);
	ft_putendl_fd(message, 2);
	errno = error_code;
	return (NULL);
}

static void	escape_char(char *line)
{
	const char	escape_chars_list[] = "\\\'\"<>()|$";
	const char	line_char = *(line + 1);

	if (ft_strchr(escape_chars_list, line_char))
		ft_memmove(line, line + 1, ft_strlen(line));
}

static char	*skip_brackets(char *line)
{
	t_opened	opened;
	int			opened_brackets_count;
	bool		words_started;

	opened = (t_opened){false, false};
	opened_brackets_count = 1;
	words_started = false;
	while (*(++line) && opened_brackets_count)
	{
		if (*line == '\\' && !opened.single_quote)
			line++;
		else if (*line == '\'' && !opened.double_quote)
			opened.single_quote = !opened.single_quote;
		else if (*line == '\"' && !opened.single_quote)
			opened.double_quote = !opened.double_quote;
		else if (*line == '(' && !opened.single_quote && !opened.double_quote)
			opened_brackets_count++;
		else if (*line == ')' && !opened.single_quote && !opened.double_quote)
			opened_brackets_count--;
		if (opened_brackets_count && !ft_isspace(*line))
			words_started = true;
	}
	if (opened_brackets_count || words_started == false)
		return (NULL);
	return (line);
}

static char	*parse_find_cmd_end_after_brackets(char *line, bool words_started)
{
	if (words_started)
		return (NULL);
	line = skip_brackets(line);
	if (line == NULL)
		return (NULL);
	while (ft_isspace(*line))
		line++;
	if (line[0] == '\0' || line[0] == '|' || (line[0] == '&' && line[1] == '&'))
		return (line);
	else
		return (NULL);
}

static char	*parse_find_cmd_end(char *line)
{
	t_opened	opened;
	bool		words_started;

	opened = (t_opened){false, false};
	words_started = false;
	while (*line && !((line[0] == '|' || (line[0] == '&' && line[1] == '&')) \
				&& !opened.single_quote && !opened.double_quote))
	{
		if (*line == '\'' && !opened.double_quote)
			opened.single_quote = !opened.single_quote;
		else if (*line == '\"' && !opened.single_quote)
			opened.double_quote = !opened.double_quote;
		else if (*line == '(' && !opened.single_quote && !opened.double_quote)
			return (parse_find_cmd_end_after_brackets(line, words_started));
		else if (*line == ')' && !opened.single_quote && !opened.double_quote)
			return (NULL);
		else if (*line == '\\' && !opened.single_quote)
			escape_char(line);
		if (!ft_isspace(*line))
			words_started = true;
		line++;
	}
	if (opened.single_quote || opened.double_quote || words_started == false)
		return (NULL);
	return (line);
}

static char	*parse_get_operator(char *line, enum e_operator *next_operator)
{
	if (line[0] == '\0')
		*next_operator = OPERATOR_NONE;
	else if (line[0] == '|' && line[1] != '|')
	{
		*next_operator = OPERATOR_PIPE;
		line += 1;
	}
	else if (line[0] == '|' && line[1] == '|')
	{
		*next_operator = OPERATOR_OR;
		line += 2;
	}
	else if (line[0] == '&' && line[1] == '&')
	{
		*next_operator = OPERATOR_AND;
		line += 2;
	}
	while (ft_isspace(*line))
		line++;
	return (line);
}

t_list	*parse_line(char *line)
{
	char	*cmd_end;
	t_list	*cmd_list;
	t_list	*new_list;
	t_cmd	*cmd;

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
