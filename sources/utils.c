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

bool	is_identhead(char c)
{
	return (ft_isalpha(c) || c == '_');
}

bool	is_identtail(char c)
{
	return (ft_isalnum(c) || c == '_');
}

bool	valid_identifier_name(const char *name)
{
	if (name == NULL || name[0] == '\0' || !is_identhead(name[0]))
		return (false);
	while (*name)
	{
		if (is_identtail(*name))
			name++;
		else
			return (false);
	}
	return (true);
}
