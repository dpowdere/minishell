/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 18:39:48 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 20:15:21 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	execute_fork_wait(t_list *childs_list, int *exit_status)
{
	pid_t	child_pid;
	int		stat_loc;

	while (childs_list)
	{
		child_pid = wait(&stat_loc);
		if (child_pid == -1)
		{
			if (errno == EINTR)
				continue ;
			error(strerror(errno), NULL, childs_list, free);
			return (false);
		}
		if (WIFEXITED(stat_loc))
			*exit_status = WEXITSTATUS(stat_loc);
		else if (WIFSIGNALED(stat_loc))
			*exit_status = ERR_CODE_SIGNAL_OFFSET + WTERMSIG(stat_loc);
		ft_lstremoveif(&childs_list, &child_pid, pid_comparator, free);
		fprintf(stderr, "exit code %d\n", *exit_status); // REMOVE ME LATER
	}
	if (errno == EINTR)
	{
		errno = 0;
		return (false);
	}
	return (true);
}

static bool	add_pid_to_childs_list(pid_t child_pid, t_list **childs_list)
{
	t_list	*child_lst;
	pid_t	*child_pid_ptr;

	child_pid_ptr = malloc(sizeof(child_pid));
	if (child_pid_ptr == NULL)
		return (false);
	*child_pid_ptr = child_pid;
	child_lst = ft_lstnew(child_pid_ptr);
	if (child_lst == NULL)
	{
		free(child_pid_ptr);
		return (false);
	}
	ft_lstadd_front(childs_list, child_lst);
	return (true);
}

static pid_t	execute_fork(t_cmd *cmd)
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
		child_pipes_setup(pipe_out_in, fd_for_stdin, cmd->heredoc);
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

static bool	execute_run(t_cmd *cmd, bool fork_builtin,
						t_list **childs_list, int *exit_status)
{
	pid_t	child_pid;

	if (is_builtin(cmd->args[0]) && fork_builtin == false)
		return (execute_builtin(cmd, exit_status));
	else
	{
		child_pid = execute_fork(cmd);
		if (child_pid == -1)
			return (false);
		return (add_pid_to_childs_list(child_pid, childs_list));
	}
}

void	execute(t_list *cmds_list, int *exit_status)
{
	t_cmd	*cmd;
	t_list	*childs_list;
	bool	fork_builtin;

	fork_builtin = false;
	childs_list = NULL;
	while (cmds_list)
	{
		cmd = get_cooked_cmd(cmds_list->content);
		if (cmd == NULL)
			return ;
		if (cmd->next_operator == OPERATOR_PIPE)
			fork_builtin = true;
		if (execute_run(cmd, fork_builtin, &childs_list, exit_status) == false)
			return ;
		if (cmd->next_operator != OPERATOR_PIPE)
		{
			fork_builtin = false;
			if (execute_fork_wait(childs_list, exit_status) == false \
		|| (cmd->next_operator == OPERATOR_OR && *exit_status == EXIT_SUCCESS) \
		|| (cmd->next_operator == OPERATOR_AND && *exit_status != EXIT_SUCCESS))
				return ;
		}
		cmds_list = cmds_list->next;
	}
}
