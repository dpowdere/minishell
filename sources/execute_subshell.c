/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_subshell.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/31 18:34:03 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/31 21:53:59 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_subshell(char **tokens)
{
	t_list	*cmds_list;
	t_list	*tokens_list;
	int		exit_status;

	if (set_env(SUBSHELL_ENV, "1") == -1)
		return (EXIT_FAILURE);
	tokens_list = get_tokens_list_subshell(tokens + 1);
	if (errno == ENOMEM)
		return (EXIT_FAILURE);
	ft_free_ptr_array((void **)tokens);
	cmds_list = get_cmds_list(tokens_list);
	if (errno == ENOMEM)
		return (EXIT_FAILURE);
	execute(cmds_list, &exit_status);
	ft_lstclear(&cmds_list, free_cmd);
	return (exit_status);
}
