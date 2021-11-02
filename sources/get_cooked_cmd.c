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

#include <dirent.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "minishell.h"

#define DOUBLE_QUOTE_IS_NOT_OPEN	0
#define DOUBLE_QUOTE_IS_OPEN		1

#define ID_START "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"
#define ID_OTH "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789"
#define DOUBLE_QUOTE_ESCAPES "\"\\$\n"
#define IFS_SPACES " \t\n"

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

t_list	*new_part(char *start, enum e_phase phase)
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

t_cooking_cursor	get_cooking_cursor(t_list *word_list, int *exit_status)
{
	t_cooking_cursor	cc;

	cc = (t_cooking_cursor){};
	cc.word_list = word_list;
	cc.part_list = word_list->content;
	cc.part = cc.part_list->content;
	cc.write_cursor = cc.part->start;
	cc.cursor = cc.write_cursor;
	cc.exit_status = exit_status;
	return (cc);
}

int	string_cooking_condition(t_cooking_cursor *cc, char *s)
{
	extern int	errno;

	if (s == NULL)
		s = cc->cursor;
	return (errno == 0 && !cc->finish_phase
		&& *s != '\0'
		&& (cc->part->exclusive_end == NULL || s < cc->part->exclusive_end));
}

t_cooking_cursor	*step_cpy(t_cooking_cursor *cc)
{
	if (string_cooking_condition(cc, NULL))
	{
		if (cc->cursor != cc->write_cursor
			&& cc->part->phase != FIELD_SPLITTING
			&& cc->part->phase != _PATHEXP_IN_VARIABLE
			&& cc->part->phase != FINAL)
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
	if (cc->cursor != cc->write_cursor
		&& cc->part->phase != FIELD_SPLITTING
		&& cc->part->phase != _PATHEXP_IN_VARIABLE
		&& cc->part->phase != FINAL)
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

void	insert_exit_status(t_cooking_cursor *cc)
{
	enum e_phase	phase;
	char			*status;

	status = get_exit_status_str(*cc->exit_status);
	step(step(cc));
	if (DEBUG_CMD_COOKING)
		printf(" exit_status[" AEC_YELLOW "%s" AEC_RESET "]", status);
	if (cc->write_cursor == cc->part->start)
	{
		cc->part->phase = FINAL;
		cc->part->start = status;
		cc->recycle_wordpart = true;
	}
	else
		ft_lstadd_back(&cc->part_list, new_part(status, FINAL));
	if (cc->inside_double_quotes)
		phase = _VARSUB_OPEN_DOUBLE_QUOTE;
	else
		phase = VARIABLE_SUBSTITUTION;
	if (*cc->cursor != '\0')
		ft_lstadd_back(&cc->part_list, new_part(cc->cursor, phase));
	cc->finish_phase = true;
}

void	cook_substitute_variable(t_cooking_cursor *cc)
{
	enum e_phase	phase;
	char			*start;
	char			*tmp;

	start = cc->cursor + 1;
	if (*start == '?')
	{
		insert_exit_status(cc);
		return ;
	}
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
		cc->recycle_wordpart = true;
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
	int			qrem;

	qrem = (cc->part->phase == QUOTE_REMOVAL
			|| cc->part->phase == _QREM_OPEN_DOUBLE_QUOTE);
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
		else if (*cc->cursor == '$' && (cc->part->phase == VARIABLE_SUBSTITUTION
				|| cc->part->phase == _VARSUB_OPEN_DOUBLE_QUOTE))
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

enum e_phase	next_phase(enum e_phase phase)
{
	if (phase == VARIABLE_SUBSTITUTION)
		phase = PATHNAME_EXPANSION;
	else if (phase == _VARSUB_OPEN_DOUBLE_QUOTE)
		phase = _PATHEXP_OPEN_DOUBLE_QUOTE;
	else if (phase == FIELD_SPLITTING)
		phase = _PATHEXP_IN_VARIABLE;
	else if (phase == PATHNAME_EXPANSION)
		phase = QUOTE_REMOVAL;
	else if (phase == _PATHEXP_OPEN_DOUBLE_QUOTE)
		phase = _QREM_OPEN_DOUBLE_QUOTE;
	else if (phase == _PATHEXP_IN_VARIABLE
		|| phase == _QREM_OPEN_DOUBLE_QUOTE
		|| phase == QUOTE_REMOVAL)
		phase = FINAL;
	return (phase);
}

void	*should_expand_pathnames_check(void *initial, void *next)
{
	t_part	*part;
	char	*cursor;
	int		valid_phase;
	int		stars_only;

	part = (t_part *)next;
	valid_phase = (part->phase == PATHNAME_EXPANSION \
					|| part->phase == _PATHEXP_IN_VARIABLE);
	cursor = part->start;
	stars_only = *cursor == '*';
	while (stars_only && *cursor != '\0' && cursor < part->exclusive_end)
		stars_only *= (*cursor++ == '*');
	*(int *)initial *= valid_phase * stars_only;
	return (initial);
}

void	*gather_next_phases(void *initial, void *next)
{
	t_part	*part;

	part = (t_part *)next;
	part->phase = next_phase(part->phase);
	*(int *)initial += part->phase;
	return (initial);
}

void	*convert_to_wordparts(void *data)
{
	return (new_part((char *)data, FINAL));
}

void	cook_wordpart_expand_pathnames(t_cooking_cursor *cc)
{
	int		should_expand;
	t_list	*star_list;

	should_expand = true;
	ft_lstreduce(cc->part_list, &should_expand, should_expand_pathnames_check);
	if (should_expand)
	{
		star_list = get_star_list();
		if (star_list != NULL)
		{
			star_list = ft_lstconv(star_list, convert_to_wordparts);
			ft_lstadd_back(&star_list, cc->word_list->next);
			ft_lstclear((t_list **)&cc->word_list->content, free);
			cc->word_list->content = star_list->content;
			cc->word_list->next = star_list->next;
			free(star_list);
			cc->part_list = cc->word_list->content;
			cc->recycle_wordpart = true;
			return ;
		}
	}
	ft_lstreduce(cc->part_list,
		&cc->need_another_traversal, gather_next_phases);
	cc->dont_change_phase = true;
	cc->part_list = ft_lstlast(cc->part_list);
}

void	cook_wordpart_split_into_fields(t_cooking_cursor *cc)
{
	t_list	*lst;

	while (string_cooking_condition(cc, NULL)
		&& ft_strchr(IFS_SPACES, *cc->cursor))
		step(cc);
	if (cc->cursor != cc->write_cursor)
	{
		lst = ft_lstdetach((t_list **)&cc->word_list->content, cc->part_list);
		cc->part->start = cc->cursor;
		cc->part->exclusive_end = NULL;
		if (cc->word_list->content == NULL)
		{
			cc->recycle_wordpart = true;
			cc->word_list->content = lst;
		}
		else
		{
			ft_lstadd_back(&cc->word_list, ft_lstnew(lst));
			cc->part_list = ft_lstlast((t_list *)cc->word_list->content);
			cc->dont_change_phase = true;
		}
		cc->finish_phase = true;
		return ;
	}
	while (string_cooking_condition(cc, NULL))
	{
		if (!ft_strchr(IFS_SPACES, *cc->cursor))
			step_cpy(cc);
		else
		{
			while (string_cooking_condition(cc, NULL)
				&& ft_strchr(IFS_SPACES, *cc->cursor))
				step(cc);
			cc->finish_phase = true;
			if (*cc->cursor == '\0' && cc->part_list->next == NULL)
				return ;
			else if (*cc->cursor == '\0' && cc->part_list->next != NULL)
			{
				ft_lstadd_back(&cc->word_list, ft_lstnew(cc->part_list->next));
				cc->part_list->next = NULL;
			}
			else
			{
				lst = ft_lstnew(new_part(cc->cursor, FIELD_SPLITTING));
				((t_list *)lst->content)->next = cc->part_list->next;
				cc->part_list->next = NULL;
				ft_lstadd_back(&cc->word_list, lst);
			}
			return ;
		}
	}
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
	if (cc->dont_change_phase)
		cc->dont_change_phase = false;
	else
	{
		if (cc->part->exclusive_end == NULL
			|| cc->part->phase == QUOTE_REMOVAL
			|| cc->part->phase == _QREM_OPEN_DOUBLE_QUOTE)
			cc->part->exclusive_end = cc->write_cursor;
		cc->part->phase = next_phase(cc->part->phase);
	}
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
		printf("\n--> phase %d, w%d [", ++cc->phase_num, word_num);
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
					printf(", w%d [", word_num);
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
	if (cc->recycle_wordpart)
		cc->recycle_wordpart = false;
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
	extern int	errno;

	return (cc->word_list && cc->part_list && !errno);
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
		printf("\nresult[" AEC_RED "%s" AEC_RESET "]", (char *)arg);
	return (arg);
}

void	no_free(void *data)
{
	(void)data;
}

// Translate parts into final words
void	*serve(void *data)
{
	t_list	*part_list;
	size_t	memsize;
	char	*string_terminator;

	part_list = (t_list *)data;
	memsize = 1;
	ft_lstreduce(part_list, &memsize, calc_memsize);
	string_terminator = malloc(memsize);
	if (string_terminator == NULL)
	{
		ft_lstclear(&part_list, no_free);
		return (NULL);
	}
	part_list = ft_lstreverse(&part_list);
	string_terminator += memsize - 1;
	*string_terminator = '\0';
	return (debug_cooked_string(
			ft_lstpopreduce(&part_list, string_terminator, populate_arg)));
}

t_list	*cook_arg(t_list *word_list, void *exit_status)
{
	char				*arg;
	t_cooking_cursor	cc;

	arg = word_list->content;
	if (DEBUG_CMD_COOKING)
		printf("\narg[" AEC_BOLD_GREEN "%s" AEC_RESET "] -->", (char *)arg);
	word_list->content = new_part(arg, VARIABLE_SUBSTITUTION);
	if (word_list->content == NULL)
	{
		word_list->content = arg;
		return (word_list);
	}
	cc = get_cooking_cursor(word_list, exit_status);
	while (wordpart_cooking_condition(&cc))
	{
		cook_wordpart(&cc);
		next_wordpart(&cc, word_list);
	}
	ft_lstconv(word_list, serve);
	free(arg);
	if (DEBUG_CMD_COOKING)
		printf("\n\n");
	return (word_list);
}

void	free_word_with_parts(void *data)
{
	ft_lstclear((t_list **)&data, free);
}

void	debug_redirect(t_redirect *r)
{
	char	*type;

	type = NULL;
	if (r->type == REDIRECT_IN)
		type = "<";
	else if (r->type == REDIRECT_IN_HEREDOC)
		type = "<<";
	else if (r->type == REDIRECT_OUT)
		type = ">";
	else if (r->type == REDIRECT_OUT_APPEND)
		type = ">>";
	else if (r->type == OPERATOR_PIPE)
		type = "|";
	printf("\nredirect[[" AEC_GREEN "%s" AEC_RESET "][" AEC_BOLD_GREEN "%s"
		AEC_RESET "]] -->", type, r->target);
}

t_list	*cook_redirect(t_list *lst, void *exit_status)
{
	t_redirect			*redirect;
	char				*s1;
	char				*s2;
	size_t				len;
	t_cooking_cursor	cc;

	redirect = lst->content;
	if (DEBUG_CMD_COOKING)
		debug_redirect(redirect);
	if (redirect->type == REDIRECT_IN_HEREDOC)
		lst->content = new_part(redirect->target, QUOTE_REMOVAL);
	else
		lst->content = new_part(redirect->target, VARIABLE_SUBSTITUTION);
	if (lst->content == NULL)
	{
		lst->content = redirect;
		return (lst);
	}
	s1 = redirect->target;
	s2 = ft_strdup(s1);
	if (s2 == NULL)
		s2 = s1;
	cc = get_cooking_cursor(lst, exit_status);
	while (wordpart_cooking_condition(&cc))
	{
		cook_wordpart(&cc);
		next_wordpart(&cc, lst);
	}
	len = 0;
	if (ft_lstsize(lst) > 1 || *(size_t *)ft_lstreduce(
			(t_list *)lst->content, &len, calc_memsize) == 0)
	{
		ft_lstclear(&lst, free_word_with_parts);
		free(redirect);
		error(s2, ERR_AMBIGUOUS_REDIRECT, NULL, NULL);
		*(int *)exit_status = 1;
	}
	else
	{
		ft_lstconv(lst, serve);
		if (DEBUG_CMD_COOKING)
			printf("\n\n");
		redirect->target = lst->content;
		lst->content = redirect;
	}
	free(s1);
	if (s2 != s1)
		free(s2);
	return (lst);
}

t_cmd	*get_cooked_cmd(t_cmd *cmd, int *exit_status)
{
	extern int	errno;
	int			check;

	check = (cmd->args_list != NULL) + (cmd->redirects != NULL);
	ft_lstpipeline1_extradata(&cmd->args_list, cook_arg, exit_status);
	ft_lstpipeline1_extradata(&cmd->redirects, cook_redirect, exit_status);
	cmd = debug_cooked_cmd(cmd);
	if (errno == ENOMEM)
		error(strerror(errno), NULL, NULL, NULL);
	if (errno || check != (cmd->args_list != NULL) + (cmd->redirects != NULL))
	{
		*exit_status = 1;
		free_cmd(cmd);
		return (NULL);
	}
	cmd->args = (char **)ft_lst_to_ptr_array(&cmd->args_list);
	if (cmd->args == NULL)
	{
		*exit_status = 1;
		free_cmd(cmd);
		return (NULL);
	}
	cmd->args_list = NULL;
	return (cmd);
}
