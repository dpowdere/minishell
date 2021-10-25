/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 17:25:50 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/16 14:42:08 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	interpret(t_state *state)
{
	t_list	*tokens_list;
	t_list	*cmds_list;

	tokens_list = get_tokens_list(state->line, &state->cmd_exit_status);
	if (state->should_free_line)
		free(state->line);
	if (check_tokens(tokens_list) == false)
	{
		state->cmd_exit_status = ERR_CODE_PARSE;
		return (0);
	}
	if (errno == ENOMEM)
		return (1);
	cmds_list = get_cmds_list(tokens_list);
	if (errno == ENOMEM)
		return (1);
	execute(cmds_list, state);
	ft_lstclear(&cmds_list, free_cmd);
	return (0);
}

void	setup_environ(void)
{
	extern char	**environ;

	environ = copy_environ(ENV_DEEP_COPY_TRUE);
	if (environ == NULL)
		exit(errno);
}

void	setup_input(t_state *state, int argc, char **argv)
{
	state->argc = argc;
	state->argv = argv;
	state->should_free_line = DO_FREE_LINE;
	if (isatty(STDIN_FILENO))
	{
		state->read_user_line = readline_stdin_tty;
		state->is_input_interactive = true;
	}
	else if (argc > 1)
	{
		state->should_free_line = DONT_FREE_LINE;
		state->read_user_line = readline_arg;
	}
	else
		state->read_user_line = readline_stdin_non_tty;
}

int	main(int argc, char **argv)
{
	t_state	state;
	int		interpret_error;

	state = (t_state){};
	setup_environ();
	setup_input(&state, argc, argv);
	setup_signal_handlers(&state);
	interpret_error = 0;
	while (interpret_error == 0 && state.read_user_line(&state) > READLINE_EOF)
		interpret_error = interpret(&state);
	clean_up(&state);
	return (errno);
}
