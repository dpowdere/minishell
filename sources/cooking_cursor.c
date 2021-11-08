/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cooking_cursor.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 08:03:39 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/05 08:03:51 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_cc	get_cooking_cursor(t_list **word_list, int *exit_status)
{
	t_cc	cc;

	cc = (t_cc){};
	cc.first_word = word_list;
	cc.word_list = *word_list;
	cc.part_list = (*word_list)->content;
	cc.part = cc.part_list->content;
	cc.write_cursor = cc.part->start;
	cc.cursor = cc.write_cursor;
	cc.exit_status = exit_status;
	return (cc);
}

t_cc	*step_cpy(t_cc *cc)
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

t_cc	*step(t_cc *cc)
{
	if (string_cooking_condition(cc, NULL))
		cc->cursor += 1;
	return (cc);
}

t_cc	*final_cpy(t_cc *cc)
{
	if (cc->cursor != cc->write_cursor
		&& cc->part->phase != FIELD_SPLITTING
		&& cc->part->phase != _PATHEXP_IN_VARIABLE
		&& cc->part->phase != FINAL)
		*cc->write_cursor = *cc->cursor;
	return (cc);
}

void	tune_cursors(t_cc *cc)
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
