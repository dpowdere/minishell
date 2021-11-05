/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 17:25:50 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/31 22:20:49 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	interpret(t_state *state)
{
	t_list	*tokens_list;
	t_list	*cmds_list;

	tokens_list = get_tokens_list(state->line, &state->exit_status);
	if (state->should_free_line)
		free(state->line);
	if (check_tokens(tokens_list) == false)
	{
		state->exit_status = ERR_CODE_PARSE;
		return (0);
	}
	if (errno == ENOMEM)
		return (1);
	cmds_list = get_cmds_list(tokens_list);
	if (errno == ENOMEM)
		return (1);
	cmds_list = ft_lstconv_xd(cmds_list, input_heredocs, &state->exit_status);
	execute(cmds_list, &state->exit_status);
	ft_lstclear(&cmds_list, free_cmd);
	return (errno != EXIT_SUCCESS);
}

void	setup_environ(void)
{
	extern char	**environ;
	extern int	errno;
	char		*shlvl_old;
	char		*shlvl_new;

	environ = copy_environ(ENV_DEEP_COPY_TRUE);
	if (environ == NULL)
		exit_with_clean(errno);
	if (set_env(SUBSHELL_ENV, "0") == -1)
		exit_with_clean(errno);
	shlvl_old = getenv("SHLVL");
	if (shlvl_old == NULL)
		shlvl_new = ft_itoa(1);
	else
		shlvl_new = ft_itoa(ft_atoi(shlvl_old) + 1);
	if (shlvl_new == NULL)
		exit_with_clean(errno);
	if (set_env("SHLVL", shlvl_new) == -1)
		exit_with_clean(errno);
	free(shlvl_new);
}

void	setup_input(t_state *state, int argc, char **argv)
{
	extern int	errno;

	state->argc = argc;
	state->argv = argv;
	state->should_free_line = DO_FREE_LINE;
	if (isatty(STDIN_FILENO))
	{
		rl_change_environment = 0;
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
	if (errno == ENOTTY)
		errno = 0;
}

int	main(int argc, char **argv)
{
	extern int	errno;
	t_state		state;
	int			fatal_error;

	state = (t_state){};
	setup_environ();
	setup_input(&state, argc, argv);
	setup_signal_handlers(&state);
	fatal_error = 0;
	while (fatal_error == 0 && state.read_user_line(&state) > READLINE_EOF)
		fatal_error = interpret(&state);
	clean_up();
	if (DEBUG_EXIT_STATUS)
	{
		printf("EXIT STATUS %d\n", state.exit_status);
		printf("ERRNO %d: %s\n", errno, strerror(errno));
	}
	ft_putchar_fd('\n', STDOUT_FILENO);
	if (state.exit_status)
		return (state.exit_status);
	else
		return (errno);
}
