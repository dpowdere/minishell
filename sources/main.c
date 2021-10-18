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

extern char	**environ;

void	debug_token_iterator(void *token)
{
	printf("'%s', ", ((t_token *)token)->string);
}

int	main(int ac, char **av)
{
	char	*line;
	t_list	*tokens_list;
//	t_list	*cmds_list;

	environ = copy_environ(ENV_DEEP_COPY_TRUE);
	if (environ == NULL)
		return (1);
	setup_sigaction();
	if (isatty(STDIN_FILENO) == false)
	{
		(void)ac; (void)av;
//		cmds_list = get_cmds_list_from_args(av);
//		execute(cmds_list);
		return (errno);
	}
	while (true)
	{
		line = read_user_line();
		if (line == NULL)
			break ;
		tokens_list = get_tokens_list(line);
		free(line);
		if (errno == ENOMEM)
			break ;

		printf("TOKENS: [");
		ft_lstiter(tokens_list, debug_token_iterator);
		printf("]\n");

		if (check_tokens(tokens_list) == false)
			continue ;
//		cmds_list = get_cmds_list(tokens_list);
		ft_lstclear(&tokens_list, free_token);
		if (errno == ENOMEM)
			break ;
//		execute(cmds_list);
	}
	return (errno);
}
