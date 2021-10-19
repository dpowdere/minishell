/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 18:36:40 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/11 18:43:15 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define CLEAR_UNDO_LIST	1

static void	sigint_handler(int signum)
{
	extern char	*rl_line_buffer;

	(void)signum;
	ft_putstr_fd("\n", STDOUT_FILENO);
	rl_on_new_line();
	rl_replace_line("", CLEAR_UNDO_LIST);
	rl_redisplay();
}

void	setup_signal_handlers(void)
{
	struct sigaction	on_quit;
	struct sigaction	on_int;
	struct termios		term_attrs;
	extern int			rl_catch_signals;

	rl_catch_signals = false;
	tcgetattr(STDOUT_FILENO, &term_attrs);
	term_attrs.c_lflag &= ~ECHOCTL;
	tcsetattr(STDIN_FILENO, TCSANOW, &term_attrs);
	ft_bzero(&on_quit, sizeof(on_quit));
	on_quit.sa_handler = SIG_IGN;
	ft_bzero(&on_int, sizeof(on_int));
	on_int.sa_handler = sigint_handler;
	if (sigaction(SIGQUIT, &on_quit, NULL) < 0
		|| sigaction(SIGTERM, &on_quit, NULL) < 0
		|| sigaction(SIGINT, &on_int, NULL) < 0)
	{
		error(ERR_ERRNO, NULL, NULL, NULL);
		exit(1);
	}
}
