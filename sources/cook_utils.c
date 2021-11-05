/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cook_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 08:29:31 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/05 08:29:34 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	string_cooking_condition(t_cc *cc, char *s)
{
	extern int	errno;

	if (s == NULL)
		s = cc->cursor;
	return (errno == 0 && !cc->finish_phase
		&& *s != '\0'
		&& (cc->part->exclusive_end == NULL || s < cc->part->exclusive_end));
}

int	wordpart_cooking_condition(t_cc *cc)
{
	extern int	errno;

	return (cc->word_list && cc->part_list && !errno);
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
