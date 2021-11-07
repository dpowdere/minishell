/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_env.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 15:40:21 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 20:49:21 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_env(char *builtin_name, char **args)
{
	extern char	**environ;

	args = environ;
	while (*args)
	{
		if (printf("%s\n", *args) < 0)
		{
			error_builtin(builtin_name, strerror(errno), NULL);
			return (EXIT_FAILURE);
		}
		args++;
	}
	return (EXIT_SUCCESS);
}

static int	builtin_export_list_envs(void)
{
	extern char	**environ;
	char		**envs;

	envs = environ;
	while (*envs)
	{
		if (printf("export %s\n", *envs) < 0)
			return (EXIT_FAILURE);
		envs++;
	}
	return (EXIT_SUCCESS);
}

static int	builtin_export_set_env(char *arg)
{
	char	*name;
	char	*eq_symbol_pos;

	eq_symbol_pos = ft_strchr(arg, '=');
	if (eq_symbol_pos)
	{
		name = ft_strdup(arg);
		if (name == NULL)
			return (EXIT_FAILURE);
		eq_symbol_pos = name + (eq_symbol_pos - arg);
		*eq_symbol_pos = '\0';
		if (set_env(name, eq_symbol_pos + 1) == -1)
		{
			free(name);
			return (EXIT_FAILURE);
		}
		free(name);
	}
	else if (valid_identifier_name(arg) == false)
	{
		errno = EINVAL;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

int	builtin_export(char *builtin_name, char **args)
{
	bool	return_value;

	return_value = EXIT_SUCCESS;
	if (*args == NULL)
	{
		if (builtin_export_list_envs() == EXIT_FAILURE)
			return (error_builtin(builtin_name, strerror(errno), NULL));
		return (EXIT_SUCCESS);
	}
	while (*args)
	{
		if (builtin_export_set_env(*args) == EXIT_FAILURE)
		{
			if (errno == EINVAL)
			{
				error_builtin(builtin_name, ERR_BUILTIN_ENV_INVALID, *args);
				return_value = EXIT_FAILURE;
			}
			else
				return (error_builtin(builtin_name, strerror(errno), NULL));
		}
		args++;
	}
	return (return_value);
}

int	builtin_unset(char *builtin_name, char **args)
{
	bool	return_value;

	return_value = EXIT_SUCCESS;
	while (*args)
	{
		if (unset_env(*args) == -1)
		{
			if (errno == EINVAL)
			{
				error_builtin(builtin_name, ERR_BUILTIN_ENV_INVALID, *args);
				return_value = EXIT_FAILURE;
			}
			else
			{
				error_builtin(builtin_name, strerror(errno), NULL);
				return (EXIT_FAILURE);
			}
		}
		args++;
	}
	return (return_value);
}
