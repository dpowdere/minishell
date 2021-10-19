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

# include <errno.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <termios.h>

# include <readline/readline.h>
# include <readline/history.h>

# include "libft.h"

# define COMMAND_NAME	"minishell"
# define PROMPT_STRING	"minishell$ "

enum e_error {
	ERR_ERRNO = 0,
	ERR_SYNTAX_EOF,
	ERR_SYNTAX_MATCHING,
	ERR_SYNTAX_TOKEN
};

# define ERR_CODE_PARSE 258

# define ERR_STR_SYNTAX_EOF "syntax error: unexpected end of file"
# define ERR_STR_SYNTAX_MATCHING "unexpected EOF while looking for matching"
# define ERR_STR_SYNTAX_TOKEN "syntax error near unexpected token"

enum {
	ENV_DEEP_COPY_FALSE = false,
	ENV_DEEP_COPY_TRUE = true
};

typedef struct s_token
{
	enum e_type {
		TOKEN_WORD = 0,
		TOKEN_OPERATOR
	}		type;
	char	*string;
}	t_token;

enum e_operator	{
	OPERATOR_NONE = 0,
	OPERATOR_PIPE,
	OPERATOR_OR,
	OPERATOR_AND,
	OPERATOR_REDIRECT_IN,
	OPERATOR_REDIRECT_IN_STOPWORD,
	OPERATOR_REDIRECT_OUT,
	OPERATOR_REDIRECT_OUT_APPEND,
	OPERATOR_SUBSHELL_IN,
	OPERATOR_SUBSHELL_OUT
};

typedef struct s_cmd
{
	char			*cmd;
	char			**args_list;
	t_list			*redirect_in;
	t_list			*redirect_out;
	enum e_operator	next_operator;
}	t_cmd;

// env.c
char	**copy_environ(bool deep_copy);
int		set_env(const char *name, const char *value);
int		unset_env(const char *name);

// signals.c
void	setup_signal_handlers(void);

// readline.c
char	*read_user_line(void);

// get_tokens.c
t_list	*get_tokens_list(const char *line);
void	free_token(void *token_content);

// check_tokens.c
bool	check_tokens(t_list *tokens_list);

// execute.c
void	execute(t_list *cmd_list);

// utils.c
int		ft_ptr_array_len(const void **ptr_array);
char	*ft_strjoin_chr(char const *s1, char const *s2, char c);
int		ft_isspace(int c);
void	*error(enum e_error type, char *extra_message, \
				t_list *list_to_free, void (*free_fn)(void*));

#endif