/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/01 22:49:16 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 22:55:53 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	*error(enum e_error type, char *extra_message,
				t_list *list_to_free, void (*free_fn)(void*))
{
	ft_putstr_fd(COMMAND_NAME ": ", STDERR_FILENO);
	if (type == ERR_ERRNO)
		ft_putstr_fd(strerror(errno), STDERR_FILENO);
	else if (type == ERR_SYNTAX_EOF)
		ft_putstr_fd(ERR_STR_SYNTAX_EOF, STDERR_FILENO);
	else if (type == ERR_SYNTAX_MATCHING)
		ft_putstr_fd(ERR_STR_SYNTAX_MATCHING, STDERR_FILENO);
	else if (type == ERR_SYNTAX_TOKEN)
		ft_putstr_fd(ERR_STR_SYNTAX_TOKEN, STDERR_FILENO);
	else if (type == ERR_COMMAND_NOT_FOUND)
		ft_putstr_fd(ERR_STR_COMMAND_NOT_FOUND, STDERR_FILENO);
	if (extra_message)
	{
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(extra_message, STDERR_FILENO);
	}
	ft_putstr_fd("\n", STDERR_FILENO);
	if (list_to_free && free_fn)
		ft_lstclear(&list_to_free, free_fn);
	return (NULL);
}

void	*exit_with_error(enum e_error type, char *extra_message,
							t_list *list_to_free, void (*free_fn)(void*))
{
	error(type, extra_message, list_to_free, free_fn);
	exit(errno);
}

int	error_builtin(char *builtin_name, char *message, char *extra_message)
{
	ft_putstr_fd(COMMAND_NAME ": ", STDERR_FILENO);
	ft_putstr_fd(builtin_name, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putstr_fd(message, STDERR_FILENO);
	if (extra_message)
	{
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(extra_message, STDERR_FILENO);
	}
	ft_putstr_fd("\n", STDERR_FILENO);
	return (EXIT_FAILURE);
}
