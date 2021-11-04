/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_debug.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/20 13:14:43 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/21 18:31:25 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_DEBUG_H
# define MINISHELL_DEBUG_H

# include "libft.h"

# ifndef DEBUG_TOKENS
#  define DEBUG_TOKENS		0
# endif
# ifndef DEBUG_RAW_CMDS
#  define DEBUG_RAW_CMDS	0
# endif
# ifndef DEBUG_COOKED_CMDS
#  define DEBUG_COOKED_CMDS	0
# endif
# ifndef DEBUG_CMD_COOKING
#  define DEBUG_CMD_COOKING	0
# endif
# ifndef DEBUG_EXIT_STATUS
#  define DEBUG_EXIT_STATUS	0
# endif

// ANSI Escape Code
# define AEC_BLUE			"\x1b[34m"
# define AEC_BOLD			"\x1b[1m"
# define AEC_BOLD_GREEN		"\x1b[32;1m"
# define AEC_BOLD_RED		"\x1b[31;1m"
# define AEC_GRAY			"\x1b[38;5;247m"
# define AEC_GREEN			"\x1b[32m"
# define AEC_RED			"\x1b[31m"
# define AEC_RESET			"\x1b[0m"
# define AEC_YELLOW			"\x1b[33m"

# define AEC_ERASE_EOL		"\x1b[0K"
# define AEC_ERASE_LINE		"\x1b[2K"
# define AEC_HIDE_CURSOR	"\x1b[?25l"
# define AEC_SHOW_CURSOR	"\x1b[?25h"

typedef struct s_cmd	t_cmd;

// debug_tokens.c
t_list	*debug_tokens(t_list *tokens_list);

// debug_cmds.c
t_list	*debug_raw_cmds(t_list *raw_cmds_list);
t_cmd	*debug_cooked_cmd(t_cmd *cooked_cmd);

#endif
