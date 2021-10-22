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

static void	parse_execute(t_state *state)
{
	t_list	*tokens_list;
	t_list	*cmds_list;

	tokens_list = get_tokens_list(state->line);
	if (state->should_free_line)
		free(state->line);
	if (errno == ENOMEM)
		return ((void)error(ERR_ERRNO, NULL, NULL, NULL));
	if (check_tokens(tokens_list) == false)
		return ;
	cmds_list = get_cmds_list(tokens_list);
	ft_lstclear(&tokens_list, free_token);
	if (errno == ENOMEM)
		return ((void)error(ERR_ERRNO, NULL, NULL, NULL));
	execute(cmds_list, state);
	//ft_lstclear(&cmds_list, free_cmd);
}

void	setup_environ(t_state *state, int argc, char **argv)
{
	state->argc = argc;
	state->argv = argv;
	state->environ = copy_environ(ENV_DEEP_COPY_TRUE);
	if (state->environ == NULL)
		exit(errno);
}

void	setup_input(t_state *state)
{
	state->should_free_line = DO_FREE_LINE;
	if (isatty(STDIN_FILENO))
	{
		state->read_user_line = readline_stdin_tty;
		state->is_input_interactive = true;
	}
	else if (state->argc > 1)
	{
		state->should_free_line = DONT_FREE_LINE;
		state->read_user_line = readline_arg;
	}
	else
		state->read_user_line = readline_stdin_non_tty;
}

int	main(int ac, char **av)
{
	t_state	state;

	ft_bzero(&state, sizeof(state));
	setup_environ(&state, ac, av);
	setup_input(&state);
	setup_signal_handlers(&state);
	while (state.read_user_line(&state) > READLINE_EOF)
		parse_execute(&state);
	clean_up(&state);
	return (errno);
}
