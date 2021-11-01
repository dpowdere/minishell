/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirects.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/30 20:30:44 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/31 00:13:39 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	redirect_heredoc_create(char *heredoc)
{
	int	pipe_out_in[2];

	if (heredoc)
		if (pipe(pipe_out_in) == -1
			|| write(pipe_out_in[1], heredoc, ft_strlen(heredoc)) == -1
			|| dup2(pipe_out_in[0], STDIN_FILENO) == -1
			|| close(pipe_out_in[0]) == -1
			|| close(pipe_out_in[1]) == -1)
			return (false);
	return (true);
}

static bool	redirect_file_create(t_redirect *redirect)
{
	const int	new_file_permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	int			open_mode;
	int			file;
	int			dup2_return_value;

	open_mode = 0;
	if (redirect->type == REDIRECT_IN)
		open_mode = O_RDONLY;
	else if (redirect->type == REDIRECT_OUT)
		open_mode = O_WRONLY | O_CREAT | O_TRUNC;
	else if (redirect->type == REDIRECT_OUT_APPEND)
		open_mode = O_WRONLY | O_CREAT | O_APPEND;
	file = open(redirect->target, open_mode, new_file_permissions);
	if (file == -1)
		return (false);
	if (redirect->type == REDIRECT_IN)
		dup2_return_value = dup2(file, STDIN_FILENO);
	else
		dup2_return_value = dup2(file, STDOUT_FILENO);
	close(file);
	if (dup2_return_value == -1)
		return (false);
	return (true);
}

bool	redirect_files(t_list *redirects_list)
{
	t_redirect	*redirect;

	while (redirects_list)
	{
		redirect = redirects_list->content;
		if (redirect_file_create(redirect) == false)
		{
			error(ERR_ERRNO, redirect->target, NULL, NULL);
			return (false);
		}
		redirects_list = redirects_list->next;
	}
	return (true);
}
