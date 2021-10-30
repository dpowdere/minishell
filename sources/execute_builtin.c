/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_builtin.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 18:21:32 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/30 18:21:42 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	*is_builtin(const char *command)
{
	int			i;
	int			(*builtin_fptr[7])(char **);
	const char	*builtins[] = {"echo",		"pwd",
								"cd",		"env",
								"export",	"unset",
								"exit",		NULL};

	builtin_fptr[0] = shell_echo;
	builtin_fptr[1] = shell_pwd;
	builtin_fptr[2] = shell_cd;
	builtin_fptr[3] = shell_env;
	builtin_fptr[4] = shell_export;
	builtin_fptr[5] = shell_unset;
	builtin_fptr[6] = shell_exit;
	i = 0;
	while (builtins[i])
	{
		if (ft_strncmp(builtins[i], command, sizeof(builtins[i])) == 0)
			return (builtin_fptr[i]);
		i++;
	}
	return (NULL);
}

bool	execute_builtin(t_cmd *cmd, int *exit_status)
{
	char	**args;

	args = (char **)ft_lst_to_ptr_array(cmd->args_list);
	if (args == NULL)
		return (false);
	cmd->args_list = NULL;
//	init_redirects();
	*exit_status = execute_builtin_run(args, *exit_status);
//	destroy_redirects();
	ft_free_ptr_array((void **)args);
	return (true);
}

int	execute_builtin_run(char **args, int current_exit_status)
{
	int	(*builtin_function)(char **);

	builtin_function = is_builtin(args[0]);
	if (builtin_function == shell_exit)
		exit(current_exit_status);
	return (builtin_function(args + 1));
}