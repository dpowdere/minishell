/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_cooked_cmd.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 23:09:06 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/21 23:10:04 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "minishell.h"

t_list	*get_star_list(void)
{
	DIR				*dp;
	struct dirent	*dir_entry;
	t_list			*lst;

	lst = NULL;
	dp = opendir(".");
	dir_entry = readdir(dp);
	while (dir_entry)
	{
		if (ft_strncmp(".", dir_entry->d_name, 1))
			ft_lstadd_front(&lst, ft_lstnew(dir_entry->d_name));
		dir_entry = readdir(dp);
	}
	return (lst);
}

void	*new_part(char *from, enum e_part_type type)
{
	t_list	*part_list;
	t_part	*part;

	part = ft_calloc(1, sizeof(*part));
	part_list = ft_lstnew(part);
	if (part == NULL || part_list == NULL)
	{
		free(part);
		return (from);
	}
	part->from = from;
	part->upto = from;
	if (type == VARIABLE)
		part->split = true;
	if (type == INITIAL_STRING || type == VARIABLE)
		part->star = true;
	if (type == INITIAL_STRING)
		part->quote = true;
	if (type == INITIAL_STRING)
		part->e_esc = ESC_OUTSIDE_DOUBLE_QUOTES;
	return (part_list);
}

t_cooking_cursor	*copy_forward(t_cooking_cursor *cc)
{
	if (cc->cursor != cc->part->upto)
		*cc->part->upto = *cc->cursor;
	cc->part->upto += 1;
	cc->cursor += 1;
	return (cc);
}

t_cooking_cursor	*esc(t_cooking_cursor *cc)
{
	if (cc->part->e_esc == ESC_OUTSIDE_DOUBLE_QUOTES
		|| (cc->part->e_esc == ESC_INSIDE_DOUBLE_QUOTES
			&& ft_strchr("\"\\$\n", *(cc->cursor + 1))))
	{
		cc->cursor += 1;
		return (copy_forward(cc));
	}
	else
		return (copy_forward(copy_forward(cc)));
}

t_cooking_cursor	*single_quotes(t_cooking_cursor *cc)
{
	cc->cursor += 1;
	while (*cc->cursor != '\0' && *cc->cursor != '\'')
		*cc->part->upto++ = *cc->cursor++;
	if (*cc->cursor)
		cc->cursor += 1;
	return (cc);
}

t_cooking_cursor	get_cooking_cursor(t_list *word_list, t_state *state)
{
	t_cooking_cursor	cc;

	cc.word_list = word_list;
	cc.part_list = word_list->content;
	cc.part = cc.part_list->content;
	cc.cursor = cc.part->upto;
	cc.state = state;
	return (cc);
}

// Make cooked word parts and new words
t_list	*cook(t_list *word_list, t_state *state)
{
	t_cooking_cursor	cc;

	cc = get_cooking_cursor(word_list, state);
	while (*cc.cursor)
	{
		if (cc.part->e_esc != ESC_DONE_OR_UNNEED && *cc.cursor == '\\')
			esc(&cc);
		else if (cc.part->quote && *cc.cursor == '\'')
			single_quotes(&cc);
		else
			copy_forward(&cc);
	}
	copy_forward(&cc);
	return (word_list);
}

// Translate parts into final words
t_list	*serve(t_list *word_list)
{
	t_list	*part_list;

	part_list = word_list->content;
	word_list->content = ((t_part *)part_list->content)->from;
	free(part_list->content);
	free(part_list);
	return (word_list);
}

t_list	*cook_arg(t_list *word_list, void *state)
{
	char	*str;

	str = word_list->content;
	word_list->content = new_part(str, INITIAL_STRING);
	if (word_list->content == str)
		return (word_list);
	return (serve(cook(word_list, (t_state *)state)));
}

void	*cook_redirect(void *data)
{
	return (data);
}

t_cmd	*get_cooked_cmd(t_cmd *cmd, t_state *state)
{
	ft_lstpipeline1_extradata(&cmd->args_list, cook_arg, state);
	ft_lstconv(&cmd->redirect_in, cook_redirect);
	ft_lstconv(&cmd->redirect_out, cook_redirect);
	if (errno == ENOMEM)
		error(ERR_ERRNO, NULL, NULL, NULL);
	else if (errno == EPROTO)
		error(ERR_AMBIGUOUS_REDIRECT, NULL, NULL, NULL);
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
