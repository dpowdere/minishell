/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 20:01:22 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 22:46:28 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	pid_comparator(const pid_t *pid, const pid_t *pid_to_find)
{
	return (*pid == *pid_to_find);
}

bool	valid_identifier_name(const char *name)
{
	if (name == NULL || name[0] == '\0' || ft_isdigit(name[0]))
		return (false);
	while (*name)
	{
		if (ft_isalnum(*name) || *name == '_')
			name++;
		else
			return (false);
	}
	return (true);
}
