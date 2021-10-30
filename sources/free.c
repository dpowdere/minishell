/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 18:51:41 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/30 21:07:50 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	clean_up(t_state *state)
{
	(void)state;
	ft_lstclear(&state->childs_list, free);
	ft_free_ptr_array((void **)environ);
}

void	free_token(void *token_content)
{
	free(((t_token *)token_content)->string);
	free(token_content);
}

void	free_redirect(void *redirect_content)
{
	free(((t_redirect *)redirect_content)->target);
	free(redirect_content);
}

void	free_cmd(void *cmd_content)
{
	t_cmd	*cmd;

	cmd = (t_cmd *)cmd_content;
	ft_lstclear(&cmd->args_list, free);
	ft_lstclear(&cmd->redirects, free_redirect);
	free(cmd->heredoc);
	free(cmd);
}
