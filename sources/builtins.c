/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 15:40:21 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 21:08:37 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define OPTION_N "-n"

int	builtin_echo(char *builtin_name, char **args)
{
	bool	put_newline;

	put_newline = true;
	while (args[0] && ft_strncmp(OPTION_N, args[0], sizeof(OPTION_N)) == 0)
	{
		put_newline = false;
		args++;
	}
	while (*args)
	{
		if (printf("%s", *args) < 0)
			return (error_builtin(builtin_name, strerror(errno), NULL));
		args++;
		if (*args)
			if (printf(" ") < 0)
				return (error_builtin(builtin_name, strerror(errno), NULL));
	}
	if (put_newline == true)
		if (printf("\n") < 0)
			return (error_builtin(builtin_name, strerror(errno), NULL));
	return (EXIT_SUCCESS);
}

int	builtin_pwd(char *builtin_name, char **args)
{
	char	cwd[PATH_MAX];

	(void)args;
	if (getcwd(cwd, PATH_MAX))
		if (printf("%s\n", cwd) > 0)
			return (EXIT_SUCCESS);
	error_builtin(builtin_name, strerror(errno), NULL);
	return (EXIT_FAILURE);
}

static void	builtin_cd_update_envs(void)
{
	char	cwd[PATH_MAX];
	char	*pwd;

	pwd = getenv("PWD");
	if (pwd)
		set_env("OLDPWD", pwd);
	if (getcwd(cwd, PATH_MAX))
		set_env("PWD", cwd);
}

int	builtin_cd(char *builtin_name, char **args)
{
	char	*target_directory;

	if (ft_ptr_array_len((void **)args) > 1)
	{
		error_builtin(builtin_name, ERR_BUILTIN_TOO_MANY_ARGS, NULL);
		return (EXIT_FAILURE);
	}
	if (args[0])
		target_directory = args[0];
	else
	{
		target_directory = getenv("HOME");
		if (target_directory == NULL)
		{
			error_builtin(builtin_name, ERR_BUILTIN_HOME_NOT_SET, NULL);
			return (EXIT_FAILURE);
		}
	}
	if (target_directory[0] != '\0')
		if (chdir(target_directory) == -1)
			return (error_builtin(builtin_name, strerror(errno), NULL));
	builtin_cd_update_envs();
	return (EXIT_SUCCESS);
}

int	builtin_exit(char *builtin_name, char **args)
{
	char			*arg;
	unsigned char	return_value;

	if (ft_ptr_array_len((void **)args) > 1)
	{
		error_builtin(builtin_name, ERR_BUILTIN_TOO_MANY_ARGS, NULL);
		return (EXIT_FAILURE);
	}
	arg = args[0];
	return_value = ft_atoi(arg);
	if (arg[0] == '+' || arg[0] == '-')
		arg++;
	while (*arg)
	{
		if (ft_isdigit(*arg) == 0)
		{
			error_builtin(builtin_name, ERR_BUILTIN_NUMERIC_REQUIRED, arg);
			return (EXIT_FAILURE);
		}
		arg++;
	}
	exit(return_value);
}
