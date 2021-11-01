/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_cooked_cmd.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 23:09:06 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/31 16:07:01 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_cmd	*get_cooked_cmd(t_cmd *cmd)
{
	cmd->args = (char **)ft_lst_to_ptr_array(&cmd->args_list);
	if (cmd->args == NULL)
		return (NULL);
	/* TODO: Cook cmd for minishell consumption, expand env vars,
	** remove quotes and escape chars, additionally split by words
	** when appropriate.
	**
	** Take $? from state->cmd_retval
	*/
	return (debug_cooked_cmd(cmd));
}

/*
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

static void	remove_escape_char(char *line, bool opened_double_quote)
{
	const char	line_char = *(line + 1);

	if (opened_double_quote == false)
		ft_memmove(line, line + 1, ft_strlen(line));
	else if (line_char == '\"' || line_char == '\\')
		ft_memmove(line, line + 1, ft_strlen(line));
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
*/
