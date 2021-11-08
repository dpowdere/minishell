/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_cmds.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 21:31:52 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/01 23:02:37 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define HEADER_TMPL	"%8s: "
#define MARGIN_TMPL	"%10s"
#define REDIR_TMPL	"%2s"

static inline void	debug_next_operator(enum e_operator op)
{
	printf("\r" HEADER_TMPL AEC_BOLD, "next op");
	if (op == REDIRECT_IN)
		printf("<");
	else if (op == REDIRECT_IN_HEREDOC)
		printf("<<");
	else if (op == REDIRECT_OUT)
		printf(">");
	else if (op == REDIRECT_OUT_APPEND)
		printf(">>");
	else if (op == OPERATOR_PIPE)
		printf("|");
	else if (op == OPERATOR_OR)
		printf("||");
	else if (op == OPERATOR_AND)
		printf("&&");
	else if (op == SUBSHELL_IN)
		printf("subshell in '('");
	else if (op == SUBSHELL_OUT)
		printf("subshell out ')'");
	else if (op == OPERATOR_NONE)
		printf("none");
	printf(AEC_RESET "\n");
}

static inline void	debug_cmd_redirect(void *data)
{
	t_redirect	*r;

	r = (t_redirect *)data;
	if (r->type == REDIRECT_IN)
		printf(REDIR_TMPL, "<");
	else if (r->type == REDIRECT_IN_HEREDOC)
		printf(REDIR_TMPL, "<<");
	else if (r->type == REDIRECT_OUT)
		printf(REDIR_TMPL, ">");
	else if (r->type == REDIRECT_OUT_APPEND)
		printf(REDIR_TMPL, ">>");
	else if (r->type == OPERATOR_PIPE)
		printf(REDIR_TMPL, "|");
	else
		printf(AEC_RED "ERROR %d" AEC_RESET, r->type);
	printf(" " AEC_YELLOW "%s" AEC_RESET "\n" MARGIN_TMPL, r->target, "");
}

static inline void	debug_args(t_list *args)
{
	printf(HEADER_TMPL AEC_YELLOW, "cmd");
	if (args == NULL)
		printf(AEC_RESET AEC_BOLD "none");
	else if (*(char *)args->content == SUBSHELL_MAGIC_BYTE)
		printf(COMMAND_NAME);
	else
		printf("%s", (char *)args->content);
	printf(AEC_RESET "\n");
	if (args == NULL)
		return ;
	args = args->next;
	printf(HEADER_TMPL, "args");
	while (args)
	{
		printf(AEC_YELLOW "%s" AEC_RESET "\n" MARGIN_TMPL, \
				(char *)args->content, "");
		args = args->next;
	}
}

inline void	debug_cmd(void *data, int ix, int is_last)
{
	const t_cmd		*cmd = data;

	if (ix > 0)
		printf(AEC_BOLD "%d:" AEC_RESET "\n", ix);
	debug_args(cmd->args_list);
	printf("\r" HEADER_TMPL, "redirs");
	ft_lstiter(cmd->redirects, debug_cmd_redirect);
	if (!cmd->redirects)
		printf("\n");
	debug_next_operator(cmd->next_operator);
	(void)is_last;
}

inline t_cmd	*debug_cooked_cmd(t_cmd *cooked_cmd)
{
	if (DEBUG_COOKED_CMDS)
	{
		printf("COOKED CMD:\n");
		if (cooked_cmd)
		{
			debug_cmd((void *)cooked_cmd, 0, true);
			if (cooked_cmd->heredoc)
			{
				printf(HEADER_TMPL AEC_BOLD, "heredoc");
				printf(AEC_YELLOW "\n" "%s" AEC_RESET "\n", cooked_cmd->heredoc);
			}
		}
		printf("\n");
	}
	return (cooked_cmd);
}
