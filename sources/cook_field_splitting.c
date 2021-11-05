/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cook_field_splitting.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 07:43:32 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/05 07:43:39 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define IFS_SPACES " \t\n"

static void	when_field_should_be_concatenated_with_prev_wordpart(t_cc *cc)
{
	t_list	*lst;

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
}

static void	when_field_should_form_new_word(t_cc *cc)
{
	t_list	*lst;

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
		lst = ft_lstnew(lstnew_wordpart(cc->cursor, FIELD_SPLITTING));
		((t_list *)lst->content)->next = cc->part_list->next;
		cc->part_list->next = NULL;
		ft_lstadd_back(&cc->word_list, lst);
	}
}

void	cook_wordpart_split_into_fields(t_cc *cc)
{
	while (string_cooking_condition(cc, NULL)
		&& ft_strchr(IFS_SPACES, *cc->cursor))
		step(cc);
	if (cc->cursor != cc->write_cursor)
	{
		when_field_should_be_concatenated_with_prev_wordpart(cc);
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
			when_field_should_form_new_word(cc);
			return ;
		}
	}
}
