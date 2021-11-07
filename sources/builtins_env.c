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

#define EXPORT_ESCAPE_CHARS "$\"\\"

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
	const char	**envs = (const char **)environ;
	char		*after_equal_symbol;

	while (*envs)
	{
		after_equal_symbol = ft_strchr(*envs, '=') + 1;
		if (write(STDOUT_FILENO, "export ", 7) < 0 \
		|| write(STDOUT_FILENO, *envs, after_equal_symbol - *envs) < 0 \
		|| write(STDOUT_FILENO, "\"", 1) < 0)
			return (EXIT_FAILURE);
		while (*after_equal_symbol)
		{
			if (ft_strchr(EXPORT_ESCAPE_CHARS, *after_equal_symbol))
				if (write(STDOUT_FILENO, "\\", 1) < 0)
					return (EXIT_FAILURE);
			if (write(STDOUT_FILENO, after_equal_symbol, 1) < 0)
				return (EXIT_FAILURE);
			after_equal_symbol++;
		}
		if (write(STDOUT_FILENO, "\"\n", 2) < 0)
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
