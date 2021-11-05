/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 17:33:58 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 23:28:41 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <signal.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <termios.h>

# include <readline/readline.h>
# include <readline/history.h>

# include "get_next_line.h"
# include "libft.h"
# include "minishell_debug.h"

# define COMMAND_NAME	"minishell"
# define PROMPT_STRING	"\x1b[32mminishell\x1b[0m$ "
# define HEREDOC_PROMPT_STRING	"heredoc> "

# define SUBSHELL_MAGIC_BYTE	'\1'
# define SUBSHELL_ENV			"MINISHELL_SUBSHELL"

# define ERR_CODE_PARSE				258
# define ERR_CODE_NOT_EXECUTABLE	126
# define ERR_CODE_NOT_FOUND			127
# define ERR_CODE_SIGNAL_OFFSET		128

# define ERR_SYNTAX_EOF			"syntax error: unexpected end of file"
# define ERR_SYNTAX_MATCHING	"unexpected EOF while looking for matching"
# define ERR_SYNTAX_TOKEN		"syntax error near unexpected token"
# define ERR_AMBIGUOUS_REDIRECT	"ambiguous redirect"
# define ERR_COMMAND_NOT_FOUND	"command not found"

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
	bool			heredoc_expand_vars;
	enum e_operator	next_operator;
}	t_cmd;

enum e_phase
{
	FINAL = 0,
	VARIABLE_SUBSTITUTION,
	_VARSUB_OPEN_DOUBLE_QUOTE,
	FIELD_SPLITTING,
	PATHNAME_EXPANSION,
	_PATHEXP_IN_VARIABLE,
	_PATHEXP_OPEN_DOUBLE_QUOTE,
	QUOTE_REMOVAL,
	_QREM_OPEN_DOUBLE_QUOTE,
};

typedef struct s_part
{
	char			*start;
	char			*exclusive_end;
	enum e_phase	phase;
}	t_part;

typedef struct s_cooking_cursor
{
	t_list	*word_list;
	t_list	*part_list;
	t_part	*part;
	char	*cursor;
	char	*write_cursor;
	int		finish_phase;
	int		recycle_wordpart;
	int		dont_change_phase;
	int		inside_double_quotes;
	int		need_another_traversal;
	int		*exit_status;
	int		phase_num;
}	t_cc;

typedef struct s_cooking_extradata
{
	t_cmd	*cmd;
	int		*exit_status;
}	t_xd;

// env.c
char			**copy_environ(bool deep_copy);
int				set_env(const char *name, const char *value);
int				unset_env(const char *name);

// signals.c
void			setup_signal_handlers(t_state *state);
bool			setup_child_signal_handlers(void);

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

// heredoc.c
void			*input_heredocs(void *cmd_data, void *extra_data);

// get_cooked_cmd.c
t_cmd			*get_cooked_cmd(t_cmd *cmd, int *exit_status);
t_list			*cook_arg(t_list *word_list, void *exit_status);
// cook_redirect.c
t_list			*cook_redirect(t_list *lst, void *exit_status);
// cooking_cursor.c
t_cc			get_cooking_cursor(t_list *word_list, int *exit_status);
t_cc			*final_cpy(t_cc *cc);
t_cc			*step(t_cc *cc);
t_cc			*step_cpy(t_cc *cc);
void			tune_cursors(t_cc *cc);
// cook_field_splitting.c
void			cook_wordpart_split_into_fields(t_cc *cc);
// cook_wordpart.c
void			cook_wordpart(t_cc *cc);
void			cook_wordpart_main(t_cc *cc, int is_double_quote_open);
void			cook_wordpart_find_string_terminator(t_cc *cc);
// cook_wordpart_utils.c
t_list			*lstnew_wordpart(char *start, enum e_phase phase);
void			next_wordpart(t_cc *cc, t_list *word_list);
void			seal_wordpart(t_cc *cc);
// cook_star.c
void			cook_wordpart_expand_pathnames(t_cc *cc);
void			*should_expand_pathnames_check(void *initial, void *next);
// cook_quotes.c
void			cook_escape(t_cc *cc);
void			cook_single_quotes(t_cc *cc);
void			cook_double_quotes(t_cc *cc, int is_double_quote_open);
// cook_variables.c
void			cook_substitute_variable(t_cc *cc);
// cook_serve.c
void			*serve(void *data);
void			*calc_memsize(void *initial, void *next);
// cook_utils.c
int				string_cooking_condition(t_cc *cc, char *s);
int				wordpart_cooking_condition(t_cc *cc);
enum e_phase	next_phase(enum e_phase phase);

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
pid_t			execute_fork(t_cmd *cmd);

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

// error.c
void			*error(char *error_message, char *extra_message, \
								t_list *list_to_free, void (*free_fn)(void*));
void			exit_with_error(t_list *list_to_free, void (*free_fn)(void*));
int				error_builtin(char *builtin_name, char *message, \
								char *extra_message);

// exit_status.c
char			*get_exit_status_str(int exit_status);
void			insert_exit_status(t_cc *cc);

// free.c
void			free_token(void *token_content);
void			free_redirect(void *redirect_content);
void			free_cmd(void *cmd_content);
void			clean_up(void);
void			exit_with_clean(int return_value);

// utils.c
int				get_exit_status(int stat_loc);
bool			file_exists(char *path);
int				pid_comparator(const pid_t *pid, const pid_t *pid_to_find);
bool			is_identhead(char c);
bool			is_identtail(char c);
bool			valid_identifier_name(const char *name);

#endif
