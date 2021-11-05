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
		cook_wordpart_expand_pathnames(cc);
	else if (phase == QUOTE_REMOVAL)
		cook_wordpart_main(cc, DOUBLE_QUOTE_IS_NOT_OPEN);
	else if (phase == _QREM_OPEN_DOUBLE_QUOTE)
		cook_wordpart_main(cc, DOUBLE_QUOTE_IS_OPEN);
	else if (phase == FINAL && cc->part->exclusive_end == NULL)
		cook_wordpart_find_string_terminator(cc);
}
