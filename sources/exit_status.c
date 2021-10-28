/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_status.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/26 21:46:59 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/26 21:47:34 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	update_cmd_exit_status(int exit_status, t_state *state)
{
	if (exit_status == state->cmd_exit_status)
		return ;
	state->cmd_exit_status = exit_status;
	free(state->cmd_exit_status_str);
	state->cmd_exit_status_str = NULL;
}

char	*get_cmd_exit_status_str(t_state *state)
{
	if (state->cmd_exit_status_str == NULL)
		state->cmd_exit_status_str = ft_itoa(state->cmd_exit_status);
	return (state->cmd_exit_status_str);
}
