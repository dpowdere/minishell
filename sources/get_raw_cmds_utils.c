/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_raw_cmds_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/31 19:53:53 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/31 19:54:20 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

enum e_operator	get_operator_type(const char *line)
{
	if (!ft_memcmp(line, "||", 2))
		return (OPERATOR_OR);
	else if (!ft_memcmp(line, "&&", 2))
		return (OPERATOR_AND);
	else if (!ft_memcmp(line, "<<", 2))
		return (REDIRECT_IN_HEREDOC);
	else if (!ft_memcmp(line, ">>", 2))
		return (REDIRECT_OUT_APPEND);
	else if (*line == '|')
		return (OPERATOR_PIPE);
	else if (*line == '<')
		return (REDIRECT_IN);
	else if (*line == '>')
		return (REDIRECT_OUT);
	else if (*line == '(')
		return (SUBSHELL_IN);
	else if (*line == ')')
		return (SUBSHELL_OUT);
	else
		return (OPERATOR_NONE);
}

t_list	*popconvert_tokenlst_to_stringlst(t_list **tokens_list)
{
	t_list	*stringlst;
	t_token	*token;

	stringlst = *tokens_list;
	token = (*tokens_list)->content;
	*tokens_list = (*tokens_list)->next;
	stringlst->content = token->string;
	stringlst->next = NULL;
	free(token);
	return (stringlst);
}
