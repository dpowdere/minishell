/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_cooked_cmd.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 23:09:06 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/31 16:07:01 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_list	*cook_arg(t_list *word_list, void *exit_status)
{
	char	*arg;

	arg = word_list->content;
	if (DEBUG_CMD_COOKING)
		printf("\narg[" AEC_BOLD_GREEN "%s" AEC_RESET "] -->", (char *)arg);
	word_list->content = lstnew_wordpart(arg, VARIABLE_SUBSTITUTION);
	if (word_list->content == NULL)
	{
		word_list->content = arg;
		return (word_list);
	}
	cook(word_list, exit_status);
	ft_lstconv(word_list, serve);
	free(arg);
	if (DEBUG_CMD_COOKING)
		printf("\n\n");
	return (word_list);
}

static void	*free_return(t_cmd *cmd, int *exit_status)
{
	*exit_status = 1;
	free_cmd(cmd);
	return (NULL);
}

t_cmd	*get_cooked_cmd(t_cmd *cmd, int *exit_status)
{
	int		check;
	t_xd	xd;

	check = (cmd->args_list != NULL) + (cmd->redirects != NULL);
	xd.cmd = cmd;
	xd.exit_status = exit_status;
	ft_lstpipeline_xd(&cmd->args_list, cook_arg, exit_status);
	ft_lstpipeline_xd(&cmd->redirects, cook_redirect, &xd);
	cmd = debug_cooked_cmd(cmd);
	if (errno == ENOMEM)
		error(strerror(errno), NULL, NULL, NULL);
	if (errno || check != (cmd->args_list != NULL) + (cmd->redirects != NULL))
		return (free_return(cmd, exit_status));
	cmd->args = (char **)ft_lst_to_ptr_array(&cmd->args_list);
	if (cmd->args == NULL)
		return (free_return(cmd, exit_status));
	cmd->args_list = NULL;
	return (cmd);
}
