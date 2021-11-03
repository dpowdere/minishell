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

static void	debug_next_operator(enum e_operator op)
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
	const t_cmd		*cmd = data;
	const t_list	*args = cmd->args_list;

	if (ix > 0)
		printf(AEC_BOLD "%d:" AEC_RESET "\n", ix);
	printf(HEADER_TMPL AEC_YELLOW, "cmd");
	if (*(char *)args->content == SUBSHELL_MAGIC_BYTE)
		printf(COMMAND_NAME);
	else
		printf("%s", (char *)args->content);
	printf(AEC_RESET "\n");
	args = args->next;
	printf(HEADER_TMPL, "args");
	while (args)
	{
		printf(AEC_YELLOW "%s" AEC_RESET "\n" MARGIN_TMPL, \
				(char *)args->content, "");
		args = args->next;
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
		ft_putstr_fd("RAW CMDS: [\n", STDERR_FILENO);
		ft_lstiterix(raw_cmds_list, debug_cmd);
		ft_putstr_fd("]\n", STDERR_FILENO);
	}
	return (raw_cmds_list);
}

inline t_cmd	*debug_cooked_cmd(t_cmd *cooked_cmd)
{
	if (DEBUG_COOKED_CMDS)
	{
		ft_putstr_fd("COOKED CMD:\n", STDERR_FILENO);
		debug_cmd((void *)cooked_cmd, 0, true);
		if (cooked_cmd->heredoc)
		{
			printf(HEADER_TMPL AEC_BOLD, "heredoc");
			printf(AEC_YELLOW "\n" "%s" AEC_RESET "\n", cooked_cmd->heredoc);
		}
	}
	return (cooked_cmd);
}
