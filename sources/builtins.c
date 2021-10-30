/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 15:40:21 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/16 15:58:25 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	shell_echo(char **args)
{
	(void)args;
	printf("echo Not Implemented\n");
	return (EXIT_SUCCESS);
}

int	shell_pwd(char **args)
{
	(void)args;
	printf("pwd Not Implemented\n");
	return (EXIT_SUCCESS);
}

int	shell_cd(char **args)
{
	(void)args;
	printf("cd Not Implemented\n");
	return (EXIT_SUCCESS);
}

//	dummy exit function
int	shell_exit(char **args)
{
	(void)args;
	exit(EXIT_SUCCESS);
}
