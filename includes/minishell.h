/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 17:33:58 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/11 19:13:04 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include "libft.h"

// setup.c
char	**setup_env(void);
void	setup_sigaction(void);

// readline.c
char	*read_user_line(void);

// parse.c
t_list	*parse_line(char *line);

// execute.c
void	execute(t_list *cmd_list, char **env);

#endif
