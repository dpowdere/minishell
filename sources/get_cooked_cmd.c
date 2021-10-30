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

#define DOUBLE_QUOTE_IS_NOT_OPEN	0
#define DOUBLE_QUOTE_IS_OPEN		1

#define ID_START "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"
#define ID_OTH "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789"
#define DOUBLE_QUOTE_ESCAPES "\"\\$\n"

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

void	*new_part(char *start, enum e_phase phase)
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
	part->start = start;
	part->exclusive_end = NULL;
	part->phase = phase;
	return (part_list);
}

t_cooking_cursor	get_cooking_cursor(t_list *word_list, t_state *state)
{
	t_cooking_cursor	cc;

	cc = (t_cooking_cursor){};
	cc.word_list = word_list;
	cc.part_list = word_list->content;
	cc.part = cc.part_list->content;
	cc.write_cursor = cc.part->start;
	cc.cursor = cc.write_cursor;
	cc.state = state;
	return (cc);
}

int	string_cooking_condition(t_cooking_cursor *cc, char *s)
{
	if (s == NULL)
		s = cc->cursor;
	return (errno == 0 && cc->error == 0 && !cc->finish_phase
		&& *s != '\0'
		&& (cc->part->exclusive_end == NULL || s < cc->part->exclusive_end));
}

t_cooking_cursor	*step_cpy(t_cooking_cursor *cc)
{
	if (string_cooking_condition(cc, NULL))
	{
		if (cc->cursor != cc->write_cursor)
			*cc->write_cursor = *cc->cursor;
		cc->write_cursor += 1;
		cc->cursor += 1;
	}
	return (cc);
}

t_cooking_cursor	*step(t_cooking_cursor *cc)
{
	if (string_cooking_condition(cc, NULL))
		cc->cursor += 1;
	return (cc);
}

t_cooking_cursor	*final_cpy(t_cooking_cursor *cc)
{
	if (cc->cursor != cc->write_cursor)
		*cc->write_cursor = *cc->cursor;
	return (cc);
}

void	cook_escape(t_cooking_cursor *cc)
{
	int	qrem;

	qrem = (cc->part->phase == QUOTE_REMOVAL
			|| cc->part->phase == _QREM_OPEN_DOUBLE_QUOTE);
	if (qrem && (!cc->inside_double_quotes
			|| ft_strchr(DOUBLE_QUOTE_ESCAPES, *(cc->cursor + 1))))
		step_cpy(step(cc));
	else
		step_cpy(step_cpy(cc));
}

void	cook_single_quotes(t_cooking_cursor *cc)
{
	int	qrem;

	qrem = (cc->part->phase == QUOTE_REMOVAL
			|| cc->part->phase == _QREM_OPEN_DOUBLE_QUOTE);
	if (qrem)
		step(cc);
	else
		step_cpy(cc);
	while (string_cooking_condition(cc, NULL) && *cc->cursor != '\'')
		step_cpy(cc);
	if (qrem)
		step(cc);
	else
		step_cpy(cc);
}

void	cook_substitute_variable(t_cooking_cursor *cc)
{
	enum e_phase	phase;
	char			*start;
	char			*tmp;

	start = cc->cursor + 1;
	if (*start == '?')
		(void)0; //insert_exit_status
	if (!string_cooking_condition(cc, start) || !ft_strchr(ID_START, *start))
	{
		step_cpy(cc);
		return ;
	}
	step(cc);
	while (string_cooking_condition(cc, NULL) && ft_strchr(ID_OTH, *cc->cursor))
		step(cc);
	tmp = ft_calloc(cc->cursor - start + 1, sizeof(char));
	if (tmp == NULL)
		return ;
	ft_strlcpy(tmp, start, cc->cursor - start + 1);
	start = getenv(tmp);
	if (DEBUG_CMD_COOKING)
	{
		printf(" env[%s=" AEC_YELLOW "%s" AEC_RESET "]", tmp, start);
	}
	free(tmp);
	if (start == NULL || ft_strlen(start) == 0)
		return ;
	if (cc->inside_double_quotes)
		phase = FINAL;
	else
		phase = FIELD_SPLITTING;
	if (cc->write_cursor == cc->part->start)
	{
		cc->part->phase = phase;
		cc->part->start = start;
		cc->recyle_wordpart = true;
	}
	else
		ft_lstadd_back(&cc->part_list, new_part(start, phase));
	if (cc->inside_double_quotes)
		phase = _VARSUB_OPEN_DOUBLE_QUOTE;
	else
		phase = VARIABLE_SUBSTITUTION;
	if (*cc->cursor != '\0')
		ft_lstadd_back(&cc->part_list, new_part(cc->cursor, phase));
	cc->finish_phase = true;
}

void	cook_double_quotes(t_cooking_cursor *cc, int is_double_quote_open)
{
	extern int	errno;
	int	qrem;

	qrem = cc->part->phase == QUOTE_REMOVAL
		|| cc->part->phase == _QREM_OPEN_DOUBLE_QUOTE;
	cc->inside_double_quotes = true;
	if (!is_double_quote_open)
	{
		if (qrem)
			step(cc);
		else
			step_cpy(cc);
	}
	while (string_cooking_condition(cc, NULL) && *cc->cursor != '"')
	{
		if (*cc->cursor == '\\')
			cook_escape(cc);
		else if (*cc->cursor == '$')
			cook_substitute_variable(cc);
		else
			step_cpy(cc);
	}
	if (string_cooking_condition(cc, NULL))
	{
		if (qrem)
			step(cc);
		else
			step_cpy(cc);
	}
	cc->inside_double_quotes = false;
}

void	cook_wordpart_substvar_qrem(t_cooking_cursor *cc,
											int is_double_quote_open)
{
	if (is_double_quote_open)
		cook_double_quotes(cc, DOUBLE_QUOTE_IS_OPEN);
	while (string_cooking_condition(cc, NULL))
	{
		if (*cc->cursor == '\\')
			cook_escape(cc);
		else if (*cc->cursor == '\'')
			cook_single_quotes(cc);
		else if (*cc->cursor == '"')
			cook_double_quotes(cc, DOUBLE_QUOTE_IS_NOT_OPEN);
		else if (*cc->cursor == '$' && cc->part->phase != QUOTE_REMOVAL
			&& cc->part->phase != _QREM_OPEN_DOUBLE_QUOTE)
			cook_substitute_variable(cc);
		else
			step_cpy(cc);
	}
}

void	cook_wordpart_find_string_terminator(t_cooking_cursor *cc)
{
	cc->cursor = ft_strchr(cc->part->start, '\0');
	cc->write_cursor = cc->cursor;
}

void	cook_wordpart_expand_pathnames(t_cooking_cursor *cc)
{
	cook_wordpart_find_string_terminator(cc);
}

void	cook_wordpart_split_into_fields(t_cooking_cursor *cc)
{
	cook_wordpart_find_string_terminator(cc);
}

void	tune_cursors(t_cooking_cursor *cc)
{
	cc->finish_phase = false;
	if (cc->part_list)
		cc->part = cc->part_list->content;
	else
		cc->part = NULL;
	if (cc->part)
	{
		cc->write_cursor = cc->part->start;
		cc->cursor = cc->write_cursor;
	}
	else
	{
		cc->write_cursor = NULL;
		cc->cursor = NULL;
	}
}

void	seal_wordpart(t_cooking_cursor *cc)
{
	final_cpy(cc);
	if (cc->part->exclusive_end == NULL
		|| cc->part->phase == QUOTE_REMOVAL
		|| cc->part->phase == _QREM_OPEN_DOUBLE_QUOTE)
		cc->part->exclusive_end = cc->write_cursor;
	if (cc->part->phase == VARIABLE_SUBSTITUTION)
		cc->part->phase = PATHNAME_EXPANSION;
	else if (cc->part->phase == _VARSUB_OPEN_DOUBLE_QUOTE)
		cc->part->phase = _PATHEXP_OPEN_DOUBLE_QUOTE;
	else if (cc->part->phase == FIELD_SPLITTING)
		cc->part->phase = _PATHEXP_IN_VARIABLE;
	else if (cc->part->phase == PATHNAME_EXPANSION)
		cc->part->phase = QUOTE_REMOVAL;
	else if (cc->part->phase == _PATHEXP_OPEN_DOUBLE_QUOTE)
		cc->part->phase = _QREM_OPEN_DOUBLE_QUOTE;
	else if (cc->part->phase == _PATHEXP_IN_VARIABLE
		|| cc->part->phase == _QREM_OPEN_DOUBLE_QUOTE
		|| cc->part->phase == QUOTE_REMOVAL)
		cc->part->phase = FINAL;
	cc->need_another_traversal += cc->part->phase;
}

void	debug_cooking_phase(t_list *word_list, t_cooking_cursor *cc)
{
	t_list	*part_list;
	t_part	*part;
	int		word_num;
	int		part_num;

	if (DEBUG_CMD_COOKING)
	{
		part_list = word_list->content;
		word_num = 1;
		part_num = 1;
		printf("\n--> phase %d, word %d [", ++cc->phase_num, word_num);
		while (word_list && part_list)
		{
			part = part_list->content;
			if (part_num > 1)
				printf(", ");
			++part_num;
			printf("`" AEC_YELLOW);
			printf("%.*s" AEC_RESET "`",
				(int)(part->exclusive_end - part->start), part->start);
			if (part_list->next != NULL)
				part_list = part_list->next;
			else
			{
				++word_num;
				part_num = 1;
				word_list = word_list->next;
				printf("]");
				if (word_list != NULL)
				{
					printf(", word %d [", word_num);
					part_list = word_list->content;
				}
				else
					part_list = NULL;
			}
		}
	}
}

void	next_wordpart(t_cooking_cursor *cc, t_list *word_list)
{
	if (cc->recyle_wordpart)
		cc->recyle_wordpart = false;
	else
	{
		seal_wordpart(cc);
		if (cc->part_list->next != NULL)
			cc->part_list = cc->part_list->next;
		else
		{
			cc->word_list = cc->word_list->next;
			if (cc->word_list == NULL)
				debug_cooking_phase(word_list, cc);
			if (cc->word_list == NULL && cc->need_another_traversal)
			{
				cc->need_another_traversal = 0;
				cc->word_list = word_list;
			}
			if (cc->word_list == NULL)
				cc->part_list = NULL;
			else
				cc->part_list = cc->word_list->content;
		}
	}
	tune_cursors(cc);
}

void	cook_wordpart(t_cooking_cursor *cc)
{
	enum e_phase	phase;

	phase = cc->part->phase;
	if (phase == VARIABLE_SUBSTITUTION)
		cook_wordpart_substvar_qrem(cc, DOUBLE_QUOTE_IS_NOT_OPEN);
	else if (phase == _VARSUB_OPEN_DOUBLE_QUOTE)
		cook_wordpart_substvar_qrem(cc, DOUBLE_QUOTE_IS_OPEN);
	else if (phase == FIELD_SPLITTING)
		cook_wordpart_split_into_fields(cc);
	else if (phase == PATHNAME_EXPANSION || phase == _PATHEXP_IN_VARIABLE
		|| phase == _PATHEXP_OPEN_DOUBLE_QUOTE)
		cook_wordpart_expand_pathnames(cc);
	else if (phase == QUOTE_REMOVAL)
		cook_wordpart_substvar_qrem(cc, DOUBLE_QUOTE_IS_NOT_OPEN);
	else if (phase == _QREM_OPEN_DOUBLE_QUOTE)
		cook_wordpart_substvar_qrem(cc, DOUBLE_QUOTE_IS_OPEN);
	else if (phase == FINAL && cc->part->exclusive_end == NULL)
		cook_wordpart_find_string_terminator(cc);
}

int	wordpart_cooking_condition(t_cooking_cursor *cc)
{
	return (cc->word_list && cc->part_list && !errno && !cc->error);
}

void	*calc_memsize(void *initial, void *next)
{
	t_part	*part;

	part = (t_part *)next;
	*(size_t *)initial += part->exclusive_end - part->start;
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
		printf("\n--> arg[" AEC_RED "%s" AEC_RESET "]\n", (char *)arg);
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
	char				*arg;
	t_cooking_cursor	cc;

	arg = word_list->content;
	if (DEBUG_CMD_COOKING)
		printf("\narg[" AEC_GREEN "%s" AEC_RESET "] -->", (char *)arg);
	word_list->content = new_part(arg, VARIABLE_SUBSTITUTION);
	if (word_list->content == NULL)
	{
		word_list->content = arg;
		return (word_list);
	}
	cc = get_cooking_cursor(word_list, state);
	while (wordpart_cooking_condition(&cc))
	{
		cook_wordpart(&cc);
		next_wordpart(&cc, word_list);
	}
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
