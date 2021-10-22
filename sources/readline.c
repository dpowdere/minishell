/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 18:41:30 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/11 18:44:16 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	readline_stdin_tty(t_state *s)
{
	static bool	readline_errno_fixed;

	s->line = readline(PROMPT_STRING);
	if (readline_errno_fixed == false)
	{
		errno = 0;
		readline_errno_fixed = true;
	}
	if (s->line == NULL)
		return (READLINE_ERROR);
	if (*s->line)
		add_history(s->line);
	return (READLINE_LINE);
}

int	readline_stdin_non_tty(t_state *s)
{
	int	retval;

	retval = get_next_line(STDIN_FILENO, &s->line);
	if (retval > 0 || (retval == 0 && ft_strlen(s->line) > 0))
		return (READLINE_LINE);
	else if (retval == 0)
	{
		free(s->line);
		return (READLINE_EOF);
	}
	return (READLINE_ERROR);
}

int	readline_arg(t_state *s)
{
	static int	i = 0;

	if (i >= s->ac)
		return (READLINE_EOF);
	if (++i < s->ac)
	{
		s->line = s->av[i];
		return (READLINE_LINE);
	}
	return (READLINE_EOF);
}
