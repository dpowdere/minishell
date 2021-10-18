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
	int		i;
	t_list	*tokens_list;

	environ = copy_environ(ENV_DEEP_COPY_TRUE);
	if (environ == NULL)
		return (1);
	setup_sigaction();
	i = 1;
	while (true)
	{
		if (ac == 1)
			line = read_user_line();
		else
			line = av[i++];
		errno = 0;
		if (line == NULL)
			break ;
		tokens_list = get_tokens_list(line);
		if (errno == ENOMEM)
			return (0);

		printf("TOKENS: [");
		ft_lstiter(tokens_list, debug_token_iterator);
		ft_lstclear(&tokens_list, free_token);
		printf("]\n");

		if (ac == 1)
			free(line);
//		execute(cmd_list);
	}
	return (0);
}
