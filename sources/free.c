/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 18:51:41 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/01 22:36:52 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	ft_ptr_array_free((void **)cmd->args);
	ft_lstclear(&cmd->redirects, free_redirect);
	free(cmd->heredoc);
	free(cmd);
}

void	clean_up(void)
{
	extern char	**environ;
	extern int	errno;
	int			errno_old;

	ft_ptr_array_free((void **)environ);
	rl_clear_history();
	errno_old = errno;
	if (isatty(STDIN_FILENO))
		ft_putchar_fd('\n', STDOUT_FILENO);
	if (errno == ENOTTY)
		errno = errno_old;
}

void	exit_with_clean(int return_value)
{
	clean_up();
	exit(return_value);
}
