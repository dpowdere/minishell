/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_cooking.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 06:39:49 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/05 06:39:59 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	next_word_or_wordpart(t_list **word_list, t_list **part_list,
									int *word_num, int *part_num)
{
	if ((*part_list)->next != NULL)
		*part_list = (*part_list)->next;
	else
	{
		++*word_num;
		*part_num = 1;
		*word_list = (*word_list)->next;
		printf("]");
		if (*word_list != NULL)
		{
			printf(", w%d [", *word_num);
			*part_list = (*word_list)->content;
		}
		else
			*part_list = NULL;
	}
}

void	debug_cooking_phase(t_list *word_list, t_cc *cc)
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
			next_word_or_wordpart(&word_list, &part_list, &word_num, &part_num);
		}
	}
}

inline void	*debug_cooked_string(void *arg)
{
	if (DEBUG_CMD_COOKING)
		printf("\nresult[" AEC_RED "%s" AEC_RESET "]", (char *)arg);
	return (arg);
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
