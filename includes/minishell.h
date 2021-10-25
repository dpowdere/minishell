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

# include "get_next_line.h"
# include "libft.h"
# include "minishell_debug.h"

# define COMMAND_NAME	"minishell"
# define PROMPT_STRING	"\x1b[32mminishell\x1b[0m$ "

enum e_error {
	ERR_ERRNO = 0,
	ERR_SYNTAX_EOF,
	ERR_SYNTAX_MATCHING,
	ERR_SYNTAX_TOKEN,
	ERR_AMBIGUOUS_REDIRECT
};

# define ERR_CODE_PARSE 258

# define ERR_STR_SYNTAX_EOF "syntax error: unexpected end of file"
# define ERR_STR_SYNTAX_MATCHING "unexpected EOF while looking for matching"
# define ERR_STR_SYNTAX_TOKEN "syntax error near unexpected token"
# define ERR_STR_AMBIGUOUS_REDIRECT "ambiguous redirect"

enum {
	ENV_DEEP_COPY_FALSE = false,
	ENV_DEEP_COPY_TRUE = true
};

enum {
	READLINE_ERROR = -1,
	READLINE_EOF,
	READLINE_LINE
};

enum {
	DONT_FREE_LINE = false,
	DO_FREE_LINE = true
};

typedef struct s_state	t_state;
typedef int				(*t_readline_func)(t_state *);
typedef struct s_state
{
	int				argc;
	char			**argv;
	char			*line;
	bool			should_free_line;
	bool			is_input_interactive;
	t_readline_func	read_user_line;
	t_list			*children_to_wait;
	int				cmd_exit_status;

}	t_state;

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

typedef struct s_redirect
{
	enum e_operator	type;
	char			*target;
}	t_redirect;

typedef struct s_cmd
{
	t_list			*args_list;
	t_list			*redirect_in;
	t_list			*redirect_out;
	enum e_operator	next_operator;
}	t_cmd;

enum e_part_type
{
	INITIAL_STRING,
	VARIABLE,
	DOUBLE_QUOTES,
	VARIABLE_IN_DOUBLE_QUOTES,
	FIELD_IN_VARIABLE
};

typedef struct s_part
{
	char	*from;
	char	*upto;
	int		parse;
	int		split;
	int		star;
	t_list	*star_list;
	int		quote;
	int		esc;
}	t_part;

typedef struct s_cooking_cursor
{
	t_list	*word_list;
	t_list	*part_list;
	t_part	*part;
	char	*cursor;
	t_state	*state;
}	t_cooking_cursor;

// env.c
char	**copy_environ(bool deep_copy);
int		set_env(const char *name, const char *value);
int		unset_env(const char *name);

// signals.c
void	setup_signal_handlers(t_state *state);

// readline.c
int		readline_arg(t_state *s);
int		readline_stdin_non_tty(t_state *s);
int		readline_stdin_tty(t_state *s);

// get_tokens.c
t_list	*get_tokens_list(const char *line);

// check_tokens.c
bool	check_tokens(t_list *tokens_list);

// get_raw_cmds.c
t_list	*get_cmds_list(t_list *tokens_list);

// get_cooked_cmd.c
t_cmd	*get_cooked_cmd(t_cmd *cmd, t_state *state);

// execute.c
void	execute(t_list *cmd_list, t_state *state);

// free.c
void	free_token(void *token_content);
void	free_redirect(void *redirect_content);
void	free_cmd(void *cmd_content);
void	clean_up(t_state *state);

// utils.c
int		ft_ptr_array_len(const void **ptr_array);
char	*ft_strjoin_chr(char const *s1, char const *s2, char c);
int		ft_isspace(int c);
void	*error(enum e_error type, char *extra_message, \
				t_list *list_to_free, void (*free_fn)(void*));

#endif
