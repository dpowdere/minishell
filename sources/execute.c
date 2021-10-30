/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 18:39:48 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/31 00:18:21 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	pid_comparator(const pid_t *pid, const pid_t *pid_to_find)
{
	return (*pid == *pid_to_find);
}

static bool	execute_fork_wait(t_state *state)
{
	pid_t	child_pid;
	int		stat_loc;

	while (state->childs_list)
	{
		child_pid = wait(&stat_loc);
		if (child_pid == -1)
		{
			if (errno == EINTR)
				continue ;
			error(ERR_ERRNO, NULL, NULL, NULL);
			ft_lstclear(&state->childs_list, free);
			return (false);
		}
		if (WIFEXITED(stat_loc))
			state->exit_status = WEXITSTATUS(stat_loc);
		else if (WIFSIGNALED(stat_loc))
			state->exit_status = ERR_CODE_SIGNAL + WTERMSIG(stat_loc);
		ft_lstremoveif(&state->childs_list, &child_pid, pid_comparator, free);
		printf("exit code %d\n", state->exit_status);
	}
	if (errno == EINTR)
		return (false);
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

static bool	execute_fork(t_cmd *cmd, t_list **childs_list)
{
	pid_t		child_pid;
	int			pipe_out_in[2];
	static int	fd_for_stdin;

	ft_bzero(pipe_out_in, sizeof(pipe_out_in));
	if (cmd->next_operator == OPERATOR_PIPE)
		if (pipe(pipe_out_in) == -1)
			return (false);
	child_pid = fork();
	if (child_pid == -1)
		return (false);
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
	return (add_pid_to_childs_list(child_pid, childs_list));
}

void	execute(t_list *cmds_list, t_state *state)
{
	t_cmd	*cmd;
	bool	fork_builtin;

	fork_builtin = false;
	while (cmds_list)
	{
//		cmd = get_cooked_cmd((t_cmd *)cursor->content, state);
		cmd = cmds_list->content;
		if (cmd->next_operator == OPERATOR_PIPE)
			fork_builtin = true;
		if (is_builtin(cmd->args_list->content) && fork_builtin == false)
		{
			if (execute_builtin(cmd, &state->exit_status) == false)
				exit_with_error(ERR_ERRNO, NULL, cmds_list, free_cmd);
		}
		else if (execute_fork(cmd, &state->childs_list) == false)
			exit_with_error(ERR_ERRNO, NULL, cmds_list, free_cmd);
		if (cmd->next_operator != OPERATOR_PIPE)
		{
			fork_builtin = false;
			if (execute_fork_wait(state) == false)
				return (ft_lstclear(&cmds_list, free_cmd));
			if ((cmd->next_operator == OPERATOR_OR && state->exit_status == 0) \
			|| (cmd->next_operator == OPERATOR_AND && state->exit_status != 0))
				return (ft_lstclear(&cmds_list, free_cmd));
		}
		free_cmd(ft_lstpop(&cmds_list));
	}
}
