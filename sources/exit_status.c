/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_status.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/26 21:46:59 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/26 21:47:34 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define INT_STRLEN_MAX	12

static inline int	get_power10(int n)
{
	int	power10;

	power10 = 1;
	n /= 10;
	while (n != 0)
	{
		n /= 10;
		power10 *= 10;
	}
	return (power10);
}

char	*get_exit_status_str(int exit_status)
{
	static int	previous_exit_status;
	static char	str[INT_STRLEN_MAX] = {'0', '\0'};
	int			power10;
	int			sign;
	int			i;

	if (exit_status != previous_exit_status)
	{
		previous_exit_status = exit_status;
		i = 0;
		sign = (exit_status >= 0) - (exit_status < 0);
		power10 = get_power10(exit_status);
		if (sign < 0)
			str[i++] = '-';
		while (power10 != 0)
		{
			str[i++] = (char)(exit_status / power10 * sign + '0');
			exit_status %= power10;
			power10 /= 10;
		}
		str[i] = '\0';
	}
	return ((char *)str);
}

void	insert_exit_status(t_cc *cc)
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
		ft_lstadd_back(&cc->part_list, lstnew_wordpart(status, FINAL));
	if (cc->inside_double_quotes)
		phase = _VARSUB_OPEN_DOUBLE_QUOTE;
	else
		phase = VARIABLE_SUBSTITUTION;
	if (*cc->cursor != '\0')
		ft_lstadd_back(&cc->part_list, lstnew_wordpart(cc->cursor, phase));
	cc->finish_phase = true;
}
