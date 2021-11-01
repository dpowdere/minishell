/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 17:33:58 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 20:20:22 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/wait.h>
# include <termios.h>

# include <readline/readline.h>
# include <readline/history.h>

# include "get_next_line.h"
# include "libft.h"
# include "minishell_debug.h"

# define COMMAND_NAME	"minishell"
# define PROMPT_STRING	"minishell$ "
# define SUBSHELL_MAGIC_BYTE '\1'
# define SUBSHELL_ENV	"MINISHELL_SUBSHELL"

enum e_error {
	ERR_ERRNO = 0,
	ERR_SYNTAX_EOF,
	ERR_SYNTAX_MATCHING,
	ERR_SYNTAX_TOKEN,
	ERR_COMMAND_NOT_FOUND
};

# define ERR_CODE_PARSE 258
# define ERR_CODE_NOT_EXECUTABLE 126
# define ERR_CODE_NOT_FOUND 127
# define ERR_CODE_SIGNAL_OFFSET 128

# define ERR_STR_SYNTAX_EOF			"syntax error: unexpected end of file"
# define ERR_STR_SYNTAX_MATCHING	"unexpected EOF while looking for matching"
# define ERR_STR_SYNTAX_TOKEN		"syntax error near unexpected token"
# define ERR_STR_COMMAND_NOT_FOUND	"command not found"

# define ERR_BUILTIN_HOME_NOT_SET 		"HOME not set"
# define ERR_BUILTIN_ENV_INVALID		"not a valid identifier"
# define ERR_BUILTIN_TOO_MANY_ARGS		"too many arguments"
# define ERR_BUILTIN_NUMERIC_REQUIRED 	"numeric argument required"

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
	int				exit_status;
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
	REDIRECT_IN,
	REDIRECT_IN_HEREDOC,
	REDIRECT_OUT,
	REDIRECT_OUT_APPEND,
	SUBSHELL_IN,
	SUBSHELL_OUT
};

typedef struct s_redirect
{
	enum e_operator	type;
	char			*target;
}	t_redirect;

typedef struct s_cmd
{
	t_list			*args_list;
	char			**args;
	t_list			*redirects;
	char			*heredoc;
	enum e_operator	next_operator;
}	t_cmd;

// env.c
char			**copy_environ(bool deep_copy);
int				set_env(const char *name, const char *value);
int				unset_env(const char *name);

// signals.c
void			setup_signal_handlers(t_state *state);

// readline.c
int				readline_arg(t_state *s);
int				readline_stdin_non_tty(t_state *s);
int				readline_stdin_tty(t_state *s);

// get_tokens.c
t_list			*get_tokens_list(const char *line, int *exit_status);
t_list			*get_tokens_list_subshell(char **tokens);

// check_tokens.c
bool			check_tokens(t_list *tokens_list);

// get_raw_cmds.c
t_list			*get_cmds_list(t_list *tokens_list);

// get_raw_cmds_utils.c
enum e_operator	get_operator_type(const char *line);
t_list			*popconvert_tokenlst_to_stringlst(t_list **tokens_list);

// get_cooked_cmd.c
t_cmd			*get_cooked_cmd(t_cmd *cmd);

// execute.c
void			execute(t_list *cmds_list, int *exit_status);

// redirects.c
bool			redirect_heredoc_create(char *heredoc);
bool			redirect_files(t_list *redirects_list);

// execute_builtin.c
void			*is_builtin(const char *command);
bool			execute_builtin(t_cmd *cmd, int *exit_status);
int				execute_builtin_run(char **args, int current_exit_status);

// execute_child.c
void			child_pipes_setup(int pipe_out_in[2], int fd_for_stdin, \
									char *heredoc);
void			execute_child(t_cmd *cmd);

// execute_subshell.c
int				execute_subshell(char **tokens);

// builtins.c
int				builtin_echo(char *builtin_name, char **args);
int				builtin_pwd(char *builtin_name, char **args);
int				builtin_cd(char *builtin_name, char **args);
int				builtin_exit(char *builtin_name, char **args);

// builtins_env.c
int				builtin_env(char *builtin_name, char **args);
int				builtin_export(char *builtin_name, char **args);
int				builtin_unset(char *builtin_name, char **args);

// free.c
void			free_token(void *token_content);
void			free_redirect(void *redirect_content);
void			free_cmd(void *cmd_content);
void			clean_up(t_state *state);

// utils.c
int				ft_ptr_array_len(void **ptr_array);
void			**ft_lst_to_ptr_array(t_list *list);
char			*ft_strjoin_chr(char const *s1, char const *s2, char c);
int				ft_isspace(int c);
char			*ft_basename(char *path);
void			*error(enum e_error type, char *extra_message, \
								t_list *list_to_free, void (*free_fn)(void*));
void			*exit_with_error(enum e_error type, char *extra_message, \
								t_list *list_to_free, void (*free_fn)(void*));
int				error_builtin(char *builtin_name, char *message, \
								char *extra_message);
int				pid_comparator(const pid_t *pid, const pid_t *pid_to_find);
bool			valid_identifier_name(const char *name);

#endif
