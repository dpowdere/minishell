/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 18:39:48 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/02 00:48:01 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	childs_wait(t_list **childs_list, int *exit_status)
{
	pid_t		child_pid;
	int			stat_loc;
	const pid_t	exit_status_child_pid = *(pid_t *)(*childs_list)->content;

	while (*childs_list)
	{
		child_pid = wait(&stat_loc);
		if (child_pid == -1)
		{
			if (errno == EINTR)
				continue ;
			error(strerror(errno), NULL, *childs_list, free);
			return (false);
		}
		if (child_pid == exit_status_child_pid)
			*exit_status = get_exit_status(stat_loc);
		ft_lstremoveif(childs_list, &child_pid, pid_comparator, free);
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

static t_list	*execute_conditional_skip(t_list *cmds_list, int exit_status)
{
	t_cmd	*cmd;

	while (cmds_list->next)
	{
		cmd = cmds_list->content;
		if (cmd->next_operator == OPERATOR_OR && exit_status != EXIT_SUCCESS)
			break ;
		if (cmd->next_operator == OPERATOR_AND && exit_status == EXIT_SUCCESS)
			break ;
		cmds_list = cmds_list->next;
	}
	return (cmds_list);
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
		cmd = get_cooked_cmd(cmds_list->content, exit_status);
		cmds_list->content = cmd;
		if (cmd == NULL)
			return ;
		if (cmd->next_operator == OPERATOR_PIPE)
			fork_builtin = true;
		if (execute_run(cmd, fork_builtin, &childs_list, exit_status) == false)
			return ;
		if (cmd->next_operator != OPERATOR_PIPE)
		{
			fork_builtin = false;
			if (childs_list && childs_wait(&childs_list, exit_status) == false)
				return ;
			cmds_list = execute_conditional_skip(cmds_list, *exit_status);
		}
		cmds_list = cmds_list->next;
	}
}
