/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cook_quotes.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 08:12:48 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/05 08:12:58 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define DOUBLE_QUOTE_ESCAPES "\"\\$\n"

void	cook_escape(t_cc *cc)
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

void	cook_single_quotes(t_cc *cc)
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

void	cook_double_quotes(t_cc *cc, int is_double_quote_open)
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
