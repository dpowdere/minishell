/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_env.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 15:40:21 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 01:05:43 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	shell_env(char **args)
{
	args = environ;
	while (*args)
		printf("%s\n", *args++);
	return (EXIT_SUCCESS);
}

static void	invalid_identifier_error(char *name)
{
	error(ERR_ENV_INVALID, name, NULL, NULL);
	errno = 0;
}

int	shell_export(char **args)
{
	char	*name;
	char	*eq_symbol_pos;

	while (*args)
	{
		eq_symbol_pos = ft_strchr(*args, '=');
		if (eq_symbol_pos)
		{
			name = ft_strdup(*args);
			if (name == NULL)
				return (EXIT_FAILURE);
			eq_symbol_pos = name + (eq_symbol_pos - *args);
			*eq_symbol_pos = '\0';
			if (set_env(name, eq_symbol_pos + 1) == -1)
			{
				if (errno == EINVAL)
					invalid_identifier_error(name);
				free(name);
				return (EXIT_FAILURE);
			}
			free(name);
		}
		args++;
	}
	return (EXIT_SUCCESS);
}

int	shell_unset(char **args)
{
	while (*args)
	{
		if (unset_env(*args) == -1)
		{
			if (errno == EINVAL)
				invalid_identifier_error(*args);
			return (EXIT_FAILURE);
		}
		args++;
	}
	return (EXIT_SUCCESS);
}
