/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 17:33:58 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/16 14:50:03 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <string.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft.h"

# define PROMPT_STRING "minishell"

# define ERR_SYNTAX "Syntax error"
# define ERR_SYNTAX_CODE 258

enum {
	ENV_DEEP_COPY_FALSE = false,
	ENV_DEEP_COPY_TRUE = true
};

enum e_operator
{
	OPERATOR_NONE,
	OPERATOR_PIPE,
	OPERATOR_AND,
	OPERATOR_OR
};

typedef struct s_cmd
{
	char			*cmd;
	char			**args_list;
	t_list			*redirect_in;
	t_list			*redirect_out;
	enum e_operator	next_operator;
}	t_cmd;

typedef struct s_opened
{
	bool	single_quote;
	bool	double_quote;
}	t_opened;

// env.c
char	**copy_environ(bool deep_copy);
int		set_env(const char *name, const char *value);
int		unset_env(const char *name);

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
char	*ft_strjoin_chr(char const *s1, char const *s2, char c);
int		ft_isspace(int c);

#endif
