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

static inline void	fatal_errno_check(t_state *state, int *fatal_error)
{
	if (state->is_input_interactive && errno == EINTR)
	{
		errno = 0;
		state->exit_status = ERR_CODE_SIGNAL_OFFSET + SIGINT;
		*fatal_error = 0;
	}
}

static int	interpret(char *line, int *exit_status)
{
	t_list	*tokens_list;
	t_list	*cmds_list;

	if (errno)
		return (1);
	tokens_list = get_tokens_list(line, exit_status);
	if (check_tokens(tokens_list) == false)
	{
		*exit_status = ERR_CODE_PARSE;
		return (0);
	}
	if (errno == ENOMEM)
		return (1);
	cmds_list = get_cmds_list(tokens_list);
	if (errno == ENOMEM)
		return (1);
	cmds_list = ft_lstconv_xd(cmds_list, input_heredocs, exit_status);
	if (errno == EINTR)
	{
		ft_lstclear(&cmds_list, free_cmd);
		return (0);
	}
	execute(cmds_list, exit_status);
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

int	main(int argc, char **argv)
{
	t_state		state;
	int			fatal_error;

	state = (t_state){};
	setup_environ();
	setup_input(&state, argc, argv);
	setup_signal_handlers(&state);
	fatal_error = 0;
	while (fatal_error == 0 && state.read_user_line(&state) > READLINE_EOF)
	{
		fatal_error = interpret(state.line, &state.exit_status);
		fatal_errno_check(&state, &fatal_error);
		if (state.should_free_line)
			free(state.line);
		if (DEBUG_EXIT_STATUS)
			printf("EXIT STATUS " AEC_RED "%d" AEC_RESET "\n"
				"ERRNO %d: %s\n", state.exit_status, errno, strerror(errno));
	}
	ft_putchar_fd('\n', STDOUT_FILENO);
	clean_up();
	if (state.exit_status)
		return (state.exit_status);
	else
		return (errno);
}
