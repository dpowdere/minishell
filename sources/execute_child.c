/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_child.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 18:21:16 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/02 00:46:42 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*get_next_env_path(char *basename)
{
	static char		path[PATH_MAX];
	static char		*path_start;
	static char		*path_end;
	const size_t	basename_len = ft_strlen(basename);

	if (path_end && *path_end == '\0')
		return (NULL);
	if (path_start == NULL)
		path_start = getenv("PATH");
	if (path_start == NULL || basename_len >= PATH_MAX - 1)
		return (NULL);
	path_end = ft_strchr(path_start, ':');
	if (path_end == NULL)
		path_end = ft_strchr(path_start, '\0');
	if (path_end - path_start < PATH_MAX - (int)basename_len)
	{
		ft_strlcpy(path, path_start, path_end - path_start + 1);
		path[path_end - path_start] = '/';
		ft_strlcpy(path + (path_end - path_start) + 1, \
					basename, basename_len + 1);
	}
	path_start = path_end + 1;
	if (file_exists(path))
		return (path);
	return (get_next_env_path(basename));
}

static void	execute_fail(char *command)
{
	if (errno == ENOENT)
	{
		errno = ERR_CODE_NOT_FOUND;
		error(ERR_COMMAND_NOT_FOUND, command, NULL, NULL);
	}
	else if (errno)
		error(strerror(errno), command, NULL, NULL);
	if (errno == EISDIR)
		errno = ERR_CODE_NOT_EXECUTABLE;
}

static void	execute_child(t_cmd *cmd)
{
	extern char	**environ;
	char		*cmd_path;

	if (setup_child_signal_handlers() == false)
		exit(EXIT_FAILURE);
	if (redirect_files(cmd->redirects) == false)
		exit(EXIT_FAILURE);
	if (is_builtin(cmd->args[0]))
		exit(execute_builtin_run(cmd->args, errno));
	if (*cmd->args[0] == SUBSHELL_MAGIC_BYTE)
		exit(execute_subshell(cmd->args));
	if (ft_strchr(cmd->args[0], '/'))
	{
		if (file_exists(cmd->args[0]))
			execve(cmd->args[0], cmd->args, environ);
	}
	else
	{
		cmd_path = get_next_env_path(cmd->args[0]);
		if (cmd_path)
			execve(cmd_path, cmd->args, environ);
	}
	execute_fail(cmd->args[0]);
	exit(errno);
}

static void	child_pipes_setup(int pipe_out_in[2], int *fd_for_stdin, char *heredoc)
{
	if (pipe_out_in[0] || pipe_out_in[1])
	{
		if (close(pipe_out_in[0]) == -1)
			exit_with_error(NULL, NULL);
		if (dup2(pipe_out_in[1], STDOUT_FILENO) == -1)
			exit_with_error(NULL, NULL);
		if (close(pipe_out_in[1]) == -1)
			exit_with_error(NULL, NULL);
	}
	if (heredoc)
	{
		if (redirect_heredoc_create(heredoc) == false)
			exit_with_error(NULL, NULL);
	}
	else if (*fd_for_stdin)
	{
		if (dup2(*fd_for_stdin, STDIN_FILENO) == -1)
			exit_with_error(NULL, NULL);
	}
	if (*fd_for_stdin)
	{
		if (close(*fd_for_stdin) == -1)
			exit_with_error(NULL, NULL);
		*fd_for_stdin = 0;
	}
}

pid_t	execute_fork(t_cmd *cmd)
{
	pid_t		child_pid;
	int			pipe_out_in[2];
	static int	fd_for_stdin;

	ft_bzero(pipe_out_in, sizeof(pipe_out_in));
	if (cmd->next_operator == OPERATOR_PIPE)
		if (pipe(pipe_out_in) == -1)
			return (-1);
	child_pid = fork();
	if (child_pid == -1)
		return (-1);
	if (child_pid == 0)
	{
		child_pipes_setup(pipe_out_in, &fd_for_stdin, cmd->heredoc);
		execute_child(cmd);
	}
	if (fd_for_stdin)
		close(fd_for_stdin);
	fd_for_stdin = 0;
	if (cmd->next_operator == OPERATOR_PIPE)
	{
		fd_for_stdin = pipe_out_in[0];
		close(pipe_out_in[1]);
	}
	return (child_pid);
}
