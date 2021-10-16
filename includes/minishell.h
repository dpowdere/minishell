/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 17:33:58 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/15 23:29:18 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft.h"

# define PROMPT_STRING "minishell"

// env.c
char	**copy_environ(bool extra_variable);
int		set_env(const char *name, const char *value);
void	unset_env(const char *name);

// setup.c
void	setup_sigaction(void);

// readline.c
char	*read_user_line(void);

// parse.c
t_list	*parse_line(char *line);

// execute.c
void	execute(t_list *cmd_list);

// utils.c
int		ft_ptr_array_len(const void **ptr_array);
void	ft_putendl_fd_array(char **str_array, int fd);
char	*ft_strjoin_chr(char const *s1, char const *s2, char c);

#endif
