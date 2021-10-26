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
#include <stddef.h>
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
	t_list	*part_list;
	char	*start;
	char	*tmp;

	start = cc->cursor + 1;
	if (*start == '\0' || !ft_strchr(ID_START, *start))
		return (copy_forward(cc));
	cc->cursor += 1;
	while (*cc->cursor != '\0' && ft_strchr(ID_OTH, *cc->cursor))
		cc->cursor += 1;
	tmp = ft_calloc(cc->cursor - start + 1, sizeof(char));
	if (tmp == NULL)
		return (cc);
	ft_strlcpy(tmp, start, cc->cursor - start + 1);
	start = getenv(tmp);
	if (DEBUG_CMD_COOKING)
		printf(" env: %s: `" AEC_GREEN "%s" AEC_RESET "`\n", tmp, start);
	free(tmp);
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
	part_list = cc->part_list;
	cc->part_list = cc->part_list->next;
	cc->part = cc->part_list->content;
	tmp = cc->cursor;
	cc->cursor = start;
	cc = process_var(cc);
	cc->cursor = tmp;
	if (*tmp != '\0')
		cc->part_list = cc->part_list->next;
	else
		cc->part_list = part_list;
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

void	*calc_memsize(void *initial, void *next)
{
	t_part	*part;

	part = (t_part *)next;
	*(size_t *)initial += part->exclusive_end - part->start;
	if (DEBUG_CMD_COOKING)
	{
		ft_putstr_fd("part: `" AEC_YELLOW, STDOUT_FILENO);
		write(STDOUT_FILENO, part->start, part->exclusive_end - part->start);
		ft_putstr_fd(AEC_RESET "` (", STDOUT_FILENO);
		ft_putnbr_fd(part->exclusive_end - part->start, STDOUT_FILENO);
		ft_putendl_fd(")", STDOUT_FILENO);
	}
	return (initial);
}

void	*populate_arg(void *initial, void *next)
{
	t_part		*part;
	ptrdiff_t	memsize;

	part = (t_part *)next;
	memsize = part->exclusive_end - part->start;
	initial = (char *)initial - memsize;
	ft_memcpy(initial, part->start, memsize);
	free(part);
	return (initial);
}

static inline void	*debug_cooked_string(void *arg)
{
	if (DEBUG_CMD_COOKING)
		printf("text: `" AEC_RED "%s" AEC_RESET "` (%ld)\n",
			(char *)arg, ft_strlen((char *)arg));
	return (arg);
}

// Translate parts into final words
void	*serve(void *data)
{
	t_list	*part_list;
	size_t	memsize;
	char	*arg_terminator;

	part_list = (t_list *)data;
	memsize = 1;
	ft_lstreduce(part_list, &memsize, calc_memsize);
	arg_terminator = malloc(memsize);
	if (arg_terminator == NULL)
	{
		ft_lstclear(&part_list, free);
		return (NULL);
	}
	part_list = ft_lstreverse(&part_list);
	arg_terminator += memsize - 1;
	*arg_terminator = '\0';
	return (debug_cooked_string(
			ft_lstpopreduce(&part_list, arg_terminator, populate_arg)));
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
