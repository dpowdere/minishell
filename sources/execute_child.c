/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_child.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 18:21:16 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/30 18:22:04 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	cmd_path_check(char *path)
{
	struct stat	command_info;

	if (stat(path, &command_info) == -1)
		return (false);
	if (S_ISDIR(command_info.st_mode))
	{
		errno = EISDIR;
		return (false);
	}
	return (true);
}

static char	*get_next_env_path(char *basename)
{
	static char		path[MAXPATHLEN];
	static char		*path_start;
	static char		*path_end;
	const size_t	basename_len = ft_strlen(basename);

	if (path_end && *path_end == '\0')
		return (NULL);
	if (path_start == NULL)
		path_start = getenv("PATH");
	if (path_start == NULL || basename_len >= MAXPATHLEN - 1)
		return (NULL);
	path_end = ft_strchr(path_start, ':');
	if (path_end == NULL)
		path_end = ft_strchr(path_start, '\0');
	if (path_end - path_start < MAXPATHLEN - (int)basename_len)
	{
		ft_strlcpy(path, path_start, path_end - path_start + 1);
		path[path_end - path_start] = '/';
		ft_strlcpy(path + (path_end - path_start) + 1, \
					basename, basename_len + 1);
	}
	path_start = path_end + 1;
	if (cmd_path_check(path) == true)
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
		error(ERR_ERRNO, command, NULL, NULL);
	if (errno == EISDIR)
		errno = ERR_CODE_NOT_EXECUTABLE;
}

void	execute_child(t_cmd *cmd)
{
	char	**args;
	char	*cmd_path;
	int		errno_execve;

	args = (char **)ft_lst_to_ptr_array(cmd->args_list);
	if (args == NULL)
		error_with_exit(ERR_ERRNO, NULL, NULL, NULL);
	if (is_builtin(args[0]))
		execute_builtin_run(args, errno);
	else if (ft_strchr(args[0], '/') && cmd_path_check(args[0]) == true)
		execve(args[0], args, environ);
	else if (ft_strchr(args[0], '/') == NULL)
	{
		cmd_path = get_next_env_path(args[0]);
		while (cmd_path)
		{
			execve(cmd_path, args, environ);
			errno_execve = errno;
			cmd_path = get_next_env_path(args[0]);
			errno = errno_execve;
		}
	}
	execute_fail(args[0]);
	ft_free_ptr_array((void **)args);
	exit(errno);
}

void	execute_child_init_streams(int pipe_out_in[2], int fd_for_stdin)
{
	if (pipe_out_in[0] || pipe_out_in[1])
	{
		close(pipe_out_in[0]);
		if (dup2(pipe_out_in[1], STDOUT_FILENO) == -1)
			error_with_exit(ERR_ERRNO, NULL, NULL, NULL);
		close(pipe_out_in[1]);
	}
	if (fd_for_stdin)
	{
		if (dup2(fd_for_stdin, STDIN_FILENO) == -1)
			error_with_exit(ERR_ERRNO, NULL, NULL, NULL);
		close(fd_for_stdin);
	}
//			redirects:
//			int file = open("file.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
//			dup2(file, STDOUT_FILENO);
//			close(file);
}
