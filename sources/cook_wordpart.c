/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cook_wordpart.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 07:11:24 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/05 07:11:29 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define DOUBLE_QUOTE_IS_NOT_OPEN	0
#define DOUBLE_QUOTE_IS_OPEN		1

void	cook_wordpart_find_string_terminator(t_cc *cc)
{
	cc->cursor = ft_strchr(cc->part->start, '\0');
	cc->write_cursor = cc->cursor;
}

void	cook_wordpart_main(t_cc *cc, int is_double_quote_open)
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

t_list	*remove_empty_wordparts(t_list *lst)
{
	t_part	*part;

	part = lst->content;
	if (part->phase == _PATHEXP_IN_VARIABLE && part->exclusive_end != NULL
		&& part->exclusive_end == part->start)
		free(ft_lstpop(&lst));
	return (lst);
}

t_cc	*cook_wordpart_remove_empty(t_cc *cc)
{
	t_list	*old_word_list;

	while (cc->word_list && cc->part_list)
	{
		ft_lstpipeline(&cc->part_list, remove_empty_wordparts);
		if (cc->part_list == NULL)
		{
			old_word_list = ft_lstdetach(cc->first_word, cc->word_list);
			cc->word_list = old_word_list->next;
			free(old_word_list);
			ft_lstadd_back(cc->first_word, cc->word_list);
			if (cc->word_list)
				cc->part_list = cc->word_list->content;
		}
		else
			break ;
	}
	tune_cursors(cc);
	if (cc->word_list == NULL)
		cc->need_another_traversal += _PATHEXP_IN_VARIABLE;
	return (cc);
}

void	cook_wordpart(t_cc *cc)
{
	enum e_phase	phase;

	phase = cc->part->phase;
	if (phase == VARIABLE_SUBSTITUTION)
		cook_wordpart_main(cc, DOUBLE_QUOTE_IS_NOT_OPEN);
	else if (phase == _VARSUB_OPEN_DOUBLE_QUOTE)
		cook_wordpart_main(cc, DOUBLE_QUOTE_IS_OPEN);
	else if (phase == FIELD_SPLITTING)
		cook_wordpart_split_into_fields(cc);
	else if (phase == PATHNAME_EXPANSION || phase == _PATHEXP_IN_VARIABLE
		|| phase == _PATHEXP_OPEN_DOUBLE_QUOTE)
		cook_wordpart_expand_pathnames(cook_wordpart_remove_empty(cc));
	else if (phase == QUOTE_REMOVAL)
		cook_wordpart_main(cc, DOUBLE_QUOTE_IS_NOT_OPEN);
	else if (phase == _QREM_OPEN_DOUBLE_QUOTE)
		cook_wordpart_main(cc, DOUBLE_QUOTE_IS_OPEN);
	else if (phase == FINAL && cc->part->exclusive_end == NULL)
		cook_wordpart_find_string_terminator(cc);
}
