/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 18:51:41 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/31 15:36:57 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	clean_up(t_state *state)
{
	extern char	**environ;

	ft_free_ptr_array((void **)environ);
	if (state->is_input_interactive)
		ft_putchar_fd('\n', STDOUT_FILENO);
}

void	free_token(void *token_content)
{
	if (token_content == NULL)
		return ;
	free(((t_token *)token_content)->string);
	free(token_content);
}

void	free_redirect(void *redirect_content)
{
	if (redirect_content == NULL)
		return ;
	free(((t_redirect *)redirect_content)->target);
	free(redirect_content);
}

void	free_cmd(void *cmd_content)
{
	t_cmd	*cmd;

	if (cmd_content == NULL)
		return ;
	cmd = (t_cmd *)cmd_content;
	ft_lstclear(&cmd->args_list, free);
	ft_free_ptr_array((void **)cmd->args);
	ft_lstclear(&cmd->redirects, free_redirect);
	free(cmd->heredoc);
	free(cmd);
}
