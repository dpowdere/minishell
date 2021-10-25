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

#define OUTSIDE_DOUBLE_QUOTES	0
#define INSIDE_DOUBLE_QUOTES	1

#define ID_START "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"
#define ID_OTH "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789"

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

void	*new_part(char *str, enum e_part_type type)
{
	t_list	*part_list;
	t_part	*part;

	part = ft_calloc(1, sizeof(*part));
	part_list = ft_lstnew(part);
	if (part == NULL || part_list == NULL)
	{
		free(part);
		return (NULL);
	}
	part->start = str;
	part->exclusive_end = str;
	if (type == VARIABLE)
		part->split = true;
	if (type == INITIAL_STRING || type == VARIABLE)
		part->star = true;
	if (type == INITIAL_STRING)
		part->quote = true;
	if (type == INITIAL_STRING)
		part->esc = true;
	return (part_list);
}

t_cooking_cursor	get_cooking_cursor(t_list *word_list, t_state *state)
{
	t_cooking_cursor	cc;

	cc = (t_cooking_cursor){};
	cc.word_list = word_list;
	cc.part_list = word_list->content;
	cc.part = cc.part_list->content;
	cc.cursor = cc.part->exclusive_end;
	cc.state = state;
	return (cc);
}

t_cooking_cursor	*copy_forward(t_cooking_cursor *cc)
{
	if (cc->cursor != cc->part->exclusive_end)
		*cc->part->exclusive_end = *cc->cursor;
	if (*cc->cursor != '\0')
	{
		cc->part->exclusive_end += 1;
		cc->cursor += 1;
	}
	return (cc);
}

t_cooking_cursor	*esc(t_cooking_cursor *cc, bool inside_double_quotes)
{
	if (!inside_double_quotes
		|| (inside_double_quotes && ft_strchr("\"\\$\n", *(cc->cursor + 1))))
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
		*cc->part->exclusive_end++ = *cc->cursor++;
	if (*cc->cursor != '\0')
		cc->cursor += 1;
	return (cc);
}

t_cooking_cursor	*double_quotes(t_cooking_cursor *cc)
{
	cc->cursor += 1;
	while (*cc->cursor != '\0' && *cc->cursor != '"')
	{
		if (*cc->cursor == '\\')
			cc = esc(cc, INSIDE_DOUBLE_QUOTES);
		else
			cc = copy_forward(cc);
	}
	if (*cc->cursor != '\0')
		cc->cursor += 1;
	return (cc);
}

t_cooking_cursor	*process_var(t_cooking_cursor *cc)
{
	while (*cc->cursor)
		copy_forward(cc);
	copy_forward(cc);
	return (cc);
}

t_cooking_cursor	*insert_var(t_cooking_cursor *cc)
{
	char	*start;
	char	*exclusive_end;
	char	*varname;

	if (!ft_strchr(ID_START, *(cc->cursor + 1)))
		return (copy_forward(cc));
	start = ++cc->cursor;
	while (ft_strchr(ID_OTH, *cc->cursor))
		cc->cursor += 1;
	exclusive_end = cc->cursor;
	varname = ft_calloc((exclusive_end - start) + 1, sizeof(char));
	if (varname == NULL)
		return (cc);
	ft_strlcpy(varname, start, (exclusive_end - start) + 1);
	start = getenv(varname);
	free(varname);
	if (start == NULL || ft_strlen(start) == 0)
		return (cc);
	cc->part_list->next = new_part(start, VARIABLE);
	if (cc->part_list->next == NULL)
		return (cc);
	if (*cc->cursor != '\0')
	{
		cc->part_list->next->next = new_part(cc->cursor, INITIAL_STRING);
		if (cc->part_list->next->next == NULL)
			return (cc);
	}
	cc->part_list = cc->part_list->next;
	cc->part = cc->part_list->content;
	cc = process_var(cc);
	cc->part_list = cc->part_list->next;
	cc->part = cc->part_list->content;
	return (cc);
}

// Make cooked word parts and new words
t_list	*cook(t_list *word_list, t_state *state)
{
	t_cooking_cursor	cc;

	cc = get_cooking_cursor(word_list, state);
	while (*cc.cursor)
	{
		if (cc.part->esc && *cc.cursor == '\\')
			esc(&cc, OUTSIDE_DOUBLE_QUOTES);
		else if (cc.part->quote && *cc.cursor == '\'')
			single_quotes(&cc);
		else if (cc.part->quote && *cc.cursor == '"')
			double_quotes(&cc);
		else if (*cc.cursor == '$')
			insert_var(&cc);
		else
			copy_forward(&cc);
	}
	copy_forward(&cc);
	return (word_list);
}

// Translate parts into final words
void	*serve(void *data)
{
	t_list	*part_list;
	t_part	*part;
	char	*arg;
	size_t	len;

	part_list = (t_list *)data;
	len = 1;
	while (part_list)
	{
		part = part_list->content;
		// count len of all parts
	}
	// malloc arg
	// cpy all parts to arg
	// replace content of the list with the arg
	free(part_list->content);
	free(part_list);
	return ();
}

t_list	*cook_arg(t_list *word_list, void *state)
{
	char	*str;

	str = word_list->content;
	word_list->content = new_part(str, INITIAL_STRING);
	if (word_list->content == NULL)
	{
		word_list->content = str;
		return (word_list);
	}
	word_list = cook(word_list, (t_state *)state);
	ft_lstconv(&word_list, serve);
	return (word_list);
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