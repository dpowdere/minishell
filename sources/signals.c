/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 18:36:40 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/20 11:13:05 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define CLEAR_UNDO_LIST	1

static void	sigint_handler(int signum)
{
	(void)signum;
	ft_putstr_fd("\n", STDOUT_FILENO);
	rl_on_new_line();
	rl_replace_line("", CLEAR_UNDO_LIST);
	rl_redisplay();
}

static void	suppress_ctrl_chars_echo(void)
{
	struct termios	tty_attrs;

	tcgetattr(STDOUT_FILENO, &tty_attrs);
	tty_attrs.c_lflag &= ~ECHOCTL;
	tcsetattr(STDIN_FILENO, TCSANOW, &tty_attrs);
}

void	setup_signal_handlers(t_state *state)
{
	struct sigaction	on_quit;
	struct sigaction	on_int;
	extern int			errno;

	if (state->is_input_interactive)
		suppress_ctrl_chars_echo();
	ft_bzero(&on_quit, sizeof(on_quit));
	ft_bzero(&on_int, sizeof(on_int));
	on_quit.sa_handler = SIG_IGN;
	on_int.sa_handler = sigint_handler;
	if (!state->is_input_interactive)
		(void)0; // TODO: Setup signal handlers for non interactive mode"
	if (sigaction(SIGQUIT, &on_quit, NULL) < 0
		|| sigaction(SIGTERM, &on_quit, NULL) < 0
		|| sigaction(SIGINT, &on_int, NULL) < 0)
	{
		error(ERR_ERRNO, NULL, NULL, NULL);
		clean_up(state);
		exit(errno);
	}
}
