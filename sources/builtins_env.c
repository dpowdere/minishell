/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_env.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 15:40:21 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/16 15:58:25 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	shell_env(char **args)
{
	extern char	**environ;
	char		**cur_env;

	(void)args;
	cur_env = environ;
	while (*cur_env)
		printf("%s\n", *cur_env++);
	return (EXIT_SUCCESS);
}

int	shell_export(char **args)
{
	(void)args;
	printf("export Not Implemented\n");
	return (EXIT_SUCCESS);
}

int	shell_unset(char **args)
{
	(void)args;
	printf("unset Not Implemented\n");
	return (EXIT_SUCCESS);
}
