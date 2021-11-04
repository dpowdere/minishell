/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 20:01:22 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 22:46:28 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	get_exit_status(int stat_loc)
{
	if (WIFEXITED(stat_loc))
		return (WEXITSTATUS(stat_loc));
	else if (WIFSIGNALED(stat_loc))
		return (ERR_CODE_SIGNAL_OFFSET + WTERMSIG(stat_loc));
	else
		return (EXIT_SUCCESS);
}

bool	file_exists(char *path)
{
	struct stat	command_info;

	if (stat(path, &command_info) == -1)
		return (false);
	if (S_ISDIR(command_info.st_mode))
	{
		errno = EISDIR;
		return (false);
	}
	return (true);
}

int	pid_comparator(const pid_t *pid, const pid_t *pid_to_find)
{
	return (*pid == *pid_to_find);
}

bool	is_identhead(char c)
{
	return (ft_isalpha(c) || c == '_');
}

bool	is_identtail(char c)
{
	return (ft_isalnum(c) || c == '_');
}

bool	valid_identifier_name(const char *name)
{
	if (name == NULL || name[0] == '\0' || !is_identhead(name[0]))
		return (false);
	while (*name)
	{
		if (is_identtail(*name))
			name++;
		else
			return (false);
	}
	return (true);
}
