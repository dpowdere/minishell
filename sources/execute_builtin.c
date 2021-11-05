/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_builtin.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 18:21:32 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 17:58:16 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	*is_builtin(const char *command)
{
	int			i;
	int			(*builtin_fptr[7])(char *, char **);
	const char	*builtins[] = {"echo",		"pwd",
								"cd",		"env",
								"export",	"unset",
								"exit",		NULL};

	builtin_fptr[0] = builtin_echo;
	builtin_fptr[1] = builtin_pwd;
	builtin_fptr[2] = builtin_cd;
	builtin_fptr[3] = builtin_env;
	builtin_fptr[4] = builtin_export;
	builtin_fptr[5] = builtin_unset;
	builtin_fptr[6] = builtin_exit;
	i = 0;
	while (builtins[i])
	{
		if (ft_strncmp(builtins[i], command, sizeof(builtins[i])) == 0)
			return (builtin_fptr[i]);
		i++;
	}
	return (NULL);
}

static bool	backup_restore_stdinout(void)
{
	static int	stdin_copy;
	static int	stdout_copy;

	if (stdin_copy == 0 || stdout_copy == 0)
	{
		stdin_copy = dup(STDIN_FILENO);
		if (stdin_copy == -1)
			return (false);
		stdout_copy = dup(STDOUT_FILENO);
		if (stdout_copy == -1)
			return (false);
	}
	else
	{
		if (dup2(stdin_copy, STDIN_FILENO) == -1)
			return (false);
		if (dup2(stdout_copy, STDOUT_FILENO) == -1)
			return (false);
		close(stdin_copy);
		close(stdout_copy);
		stdin_copy = 0;
		stdout_copy = 0;
	}
	return (true);
}

bool	execute_builtin(t_cmd *cmd, int *exit_status)
{
	if (cmd->heredoc || cmd->redirects)
		if (backup_restore_stdinout() == false)
			return (false);
	if (redirect_heredoc_create(cmd->heredoc) == false)
	{
		backup_restore_stdinout();
		return (false);
	}
	if (redirect_files(cmd->redirects) == true)
		*exit_status = execute_builtin_run(cmd->args, *exit_status);
	if (errno != ENOMEM)
		errno = 0;
	if (cmd->heredoc || cmd->redirects)
		if (backup_restore_stdinout() == false)
			return (false);
	return (true);
}

int	execute_builtin_run(char **args, const int current_exit_status)
{
	int	(*builtin_function)(char *, char **);

	builtin_function = is_builtin(args[0]);
	if (builtin_function == builtin_exit && args[1] == NULL)
		exit_with_clean(current_exit_status);
	return (builtin_function(args[0], args + 1));
}
