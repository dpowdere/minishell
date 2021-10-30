/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_cmds.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 21:31:52 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/30 21:14:13 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define HEADER_TMPL	"%8s: "
#define MARGIN_TMPL	"%10s"
#define REDIR_TMPL	"%2s"

static void	debug_next_operator(enum e_operator op)
{
	printf("\r" HEADER_TMPL AEC_BOLD, "nxt op");
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

static void	debug_redirect(void *data)
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

static void	debug_cmd(void *data, int ix, int is_last)
{
	t_cmd	*cmd;
	t_list	*arg;

	cmd = (t_cmd *)data;
	if (ix > 0)
		printf(AEC_BOLD "%d:" AEC_RESET "\n", ix);
	arg = cmd->args_list;
	if (!arg)
		printf("\n");
	printf(HEADER_TMPL AEC_YELLOW, "cmd");
	if (*(char *)arg->content == SUBSHELL_MAGIC_BYTE)
		printf(COMMAND_NAME);
	else
		printf("%s", (char *)arg->content);
	printf(AEC_RESET "\n");
	arg = arg->next;
	printf(HEADER_TMPL, "args");
	while (arg)
	{
		printf(AEC_YELLOW "%s" AEC_RESET "\n" MARGIN_TMPL,
			(char *)arg->content, "");
		arg = arg->next;
	}
	printf("\r" HEADER_TMPL, "redirs");
	ft_lstiter(cmd->redirects, debug_redirect);
	if (!cmd->redirects)
		printf("\n");
	debug_next_operator(cmd->next_operator);
	(void)is_last;
}

inline t_list	*debug_raw_cmds(t_list *raw_cmds_list)
{
	if (DEBUG_RAW_CMDS)
	{
		printf("RAW CMDS: [\n");
		ft_lstiterix(raw_cmds_list, debug_cmd);
		printf("]\n");
	}
	return (raw_cmds_list);
}

inline t_cmd	*debug_cooked_cmd(t_cmd *cooked_cmd)
{
	if (DEBUG_COOKED_CMDS)
	{
		printf("COOKED CMD:\n");
		debug_cmd((void *)cooked_cmd, 0, true);
	}
	return (cooked_cmd);
}
