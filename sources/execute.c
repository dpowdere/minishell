/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 18:39:48 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/11 22:05:52 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	run_builtin(char **args)
{
	(void)args;
	return ;
}

bool	is_builtin(char *command)
{
	int			i;
	const char	*builtins[] = {"echo",
								"pwd",
								"cd",
								"env",
								"export",
								"unset",
								"exit",
								NULL};

	i = 0;
	while (builtins[i])
	{
		if (ft_strncmp(builtins[i], command, sizeof(builtins[i])) == 0)
			return (true);
		i++;
	}
	return (false);
}

static int	child_pid_comparator(pid_t *child_pid, pid_t *pid_to_find)
{
	return (*child_pid == *pid_to_find);
}

void	execute(t_list *cmds_list, t_state *state)
{
	extern char	**environ;
	t_cmd		*cmd;
	char		**args;
	int			pipe_out_in[2];
	int			fd_to_replace_stdin;
	pid_t		child_pid;
	int			stat_loc;
	char		*cmd_path;

	fd_to_replace_stdin = 0;
	while (cmds_list)
	{
//		cmd = get_cooked_cmd((t_cmd *)cursor->content, state);
		cmd = cmds_list->content;
		ft_bzero(pipe_out_in, sizeof(pipe_out_in));
		if (cmd->next_operator == OPERATOR_PIPE)
			if (pipe(pipe_out_in) == -1)
				return (void)(error(ERR_ERRNO, NULL, cmds_list, free_cmd));
		printf("created PIPE %d -> %d\n", pipe_out_in[1], pipe_out_in[0]);
		child_pid = fork();
		if (child_pid == -1)
			exit(errno);
		if (child_pid == 0)
		{
			printf("i am child\n");
			if (fd_to_replace_stdin)
			{
				if (dup2(fd_to_replace_stdin, STDIN_FILENO) == -1)
					exit(errno);
				close(fd_to_replace_stdin);
				printf("I set stdin pipe from fd %d and closed it\n", fd_to_replace_stdin);
			}
			if (pipe_out_in[0])
			{
				close(pipe_out_in[0]);
				printf("child closed fd %d\n", pipe_out_in[0]);
			}
			if (cmd->next_operator == OPERATOR_PIPE)
			{
				printf("I set stdout pipe to fd %d and closed it\n", pipe_out_in[1]);
				if (dup2(pipe_out_in[1], STDOUT_FILENO) == -1)
					exit(errno);
				close(pipe_out_in[1]);
			}
//			redirects:
//			int file = open("file.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
//			dup2(file, STDOUT_FILENO);
//			close(file);

			args = (char **)ft_lst_to_ptr_array(cmd->args_list);
			if (args == NULL)
				return (void)(error(ERR_ERRNO, NULL, cmds_list, free_cmd));
			cmd->args_list = NULL;
			if (is_builtin(args[0]))
				run_builtin(args);
			else
			{
//				cmd_path = get_cmd_path(args[0]);
				cmd_path = args[0];
				if (errno == ENOMEM)
					;
				else if (cmd_path == NULL)
				{
					errno = ERR_CODE_NOT_FOUND;
					error(ERR_COMMAND_NOT_FOUND, args[0], NULL, NULL);
				}
				else if (execve(cmd_path, args, environ) == -1)
					error(ERR_ERRNO, NULL, NULL, NULL);
			}
			ft_free_ptr_array((void **)args);
			ft_lstclear(&cmds_list, free_cmd);
			clean_up(state);
			exit(errno);
		}
		if (fd_to_replace_stdin)
		{
			close(fd_to_replace_stdin);
			printf("parent closed in fd %d\n", fd_to_replace_stdin);
			fd_to_replace_stdin = 0;
		}
		if (pipe_out_in[1])
		{
			fd_to_replace_stdin = pipe_out_in[0];
			close(pipe_out_in[1]);
			printf("parent closed out fd %d\n", pipe_out_in[1]);
		}
		ft_lstadd_front(&state->children_to_wait, ft_lstnew(malloc(sizeof(pid_t))));
		*((int *)state->children_to_wait->content) = child_pid;
		if (cmd->next_operator != OPERATOR_PIPE)
		{
			while (state->children_to_wait)
			{
				child_pid = wait(&stat_loc);
				if (child_pid == -1)
				{
					if (errno == EINTR)
					{
						errno = 0;
						continue ;
					}
					printf("fail wait!!\n%s\n", strerror(errno));
					exit(errno);
				}
				printf("child pid %d done!\n", child_pid);
				ft_lstremoveif(&state->children_to_wait, &child_pid, child_pid_comparator, free);
				state->cmd_exit_status = WEXITSTATUS(stat_loc);
				printf("exit code %d\n", state->cmd_exit_status);
			}
		}
		free_cmd(ft_lstpop(&cmds_list));
	}
}
