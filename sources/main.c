/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 17:25:50 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/11 18:29:29 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int ac, char **av)
{
	char	*line;
	char	**env;
	int		i;
	t_list	*cmd_list;

	env = setup_env();
	setup_sigaction();
	i = 1;
	while (true)
	{
		if (ac == 1)
			line = read_user_line();
		else
			line = av[i++];
		if (line == NULL)
			break ;
		cmd_list = parse_line(line, env);
		execute(cmd_list);
	}
	return (0);
}
