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

#define INT_STR_LEN	12

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
	static char	str[INT_STR_LEN] = {'0', '\0'};
	int			power10;
	int			sign;
	int			i;

	if (exit_status != previous_exit_status)
	{
		previous_exit_status = exit_status;
		i = 0;
		sign = -1 * (exit_status < 0) + (exit_status >= 0);
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
