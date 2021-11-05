/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cook_wordpart_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 07:43:20 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/05 07:43:28 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_list	*lstnew_wordpart(char *start, enum e_phase phase)
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

void	next_wordpart(t_cc *cc, t_list *word_list)
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

void	seal_wordpart(t_cc *cc)
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
