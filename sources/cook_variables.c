/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cook_variables.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 08:16:14 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/05 08:16:27 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_identhead(char c)
{
	return (ft_isalpha(c) || c == '_');
}

bool	is_identtail(char c)
{
	return (ft_isalnum(c) || c == '_');
}

static void	update_wordparts(t_cc *cc, char *start)
{
	enum e_phase	phase;

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
		ft_lstadd_back(&cc->part_list, lstnew_wordpart(start, phase));
	if (cc->inside_double_quotes)
		phase = _VARSUB_OPEN_DOUBLE_QUOTE;
	else
		phase = VARIABLE_SUBSTITUTION;
	if (*cc->cursor != '\0')
		ft_lstadd_back(&cc->part_list, lstnew_wordpart(cc->cursor, phase));
	cc->finish_phase = true;
}

void	cook_substitute_variable(t_cc *cc)
{
	char			*start;
	char			*tmp;

	start = cc->cursor + 1;
	if (*start == '?')
		return ((void)insert_exit_status(cc));
	if (!string_cooking_condition(cc, start) || !is_identhead(*start))
		return ((void)step_cpy(cc));
	step(cc);
	while (string_cooking_condition(cc, NULL) && is_identtail(*cc->cursor))
		step(cc);
	tmp = ft_calloc(cc->cursor - start + 1, sizeof(char));
	if (tmp == NULL)
		return ;
	ft_strlcpy(tmp, start, cc->cursor - start + 1);
	start = getenv(tmp);
	if (DEBUG_CMD_COOKING)
		printf(" env[%s=" AEC_YELLOW "%s" AEC_RESET "]", tmp, start);
	free(tmp);
	if (start == NULL || ft_strlen(start) == 0)
		return ;
	update_wordparts(cc, start);
}
