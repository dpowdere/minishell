/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_cmds.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/21 21:31:52 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/21 21:32:06 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define HEADER_TMPL	"%8s: "
#define MARGIN_TMPL	"%10s"
#define REDIR_TMPL	"%2s"

static void	debug_next_operator(enum e_operator op)
{
	printf("\r" HEADER_TMPL AEC_BOLD, "nxt op");
	if (op == OPERATOR_REDIRECT_IN)
		printf("<");
	else if (op == OPERATOR_REDIRECT_IN_STOPWORD)
		printf("<<");
	else if (op == OPERATOR_REDIRECT_OUT)
		printf(">");
	else if (op == OPERATOR_REDIRECT_OUT_APPEND)
		printf(">>");
	else if (op == OPERATOR_PIPE)
		printf("|");
	else if (op == OPERATOR_OR)
		printf("||");
	else if (op == OPERATOR_AND)
		printf("&&");
	else if (op == OPERATOR_SUBSHELL_IN)
		printf("subshell in '('");
	else if (op == OPERATOR_SUBSHELL_OUT)
		printf("subshell out ')'");
	else if (op == OPERATOR_NONE)
		printf("none");
	printf(AEC_RESET "\n");
}

static void	debug_redirect(void *data)
{
	t_redirect	*r;

	r = (t_redirect *)data;
	if (r->type == OPERATOR_REDIRECT_IN)
		printf(REDIR_TMPL, "<");
	else if (r->type == OPERATOR_REDIRECT_IN_STOPWORD)
		printf(REDIR_TMPL, "<<");
	else if (r->type == OPERATOR_REDIRECT_OUT)
		printf(REDIR_TMPL, ">");
	else if (r->type == OPERATOR_REDIRECT_OUT_APPEND)
		printf(REDIR_TMPL, ">>");
	else if (r->type == OPERATOR_PIPE)
		printf(REDIR_TMPL, "|");
	else
		printf(AEC_RED "ERROR %d" AEC_RESET, r->type);
	printf(" " AEC_YELLOW "%s" AEC_RESET "\n" MARGIN_TMPL, r->word, "");
}

static void	debug_cmd(void *data, int ix, int is_last)
{
	t_cmd	*cmd;
	t_list	*arg;

	cmd = (t_cmd *)data;
	if (ix > 0)
		printf(AEC_BOLD "%d:" AEC_RESET "\n", ix);
	printf(HEADER_TMPL AEC_YELLOW "%s" AEC_RESET "\n", "cmd", cmd->cmd);
	printf(HEADER_TMPL, "args");
	arg = cmd->args_list;
	if (!arg)
		printf("\n");
	while (arg)
	{
		printf(AEC_YELLOW "%s" AEC_RESET "\n" MARGIN_TMPL,
			(char *)arg->content, "");
		arg = arg->next;
	}
	printf("\r" HEADER_TMPL, "ins");
	ft_lstiter(cmd->redirect_in, debug_redirect);
	if (!cmd->redirect_in)
		printf("\n");
	printf("\r" HEADER_TMPL, "outs");
	ft_lstiter(cmd->redirect_out, debug_redirect);
	if (!cmd->redirect_out)
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
