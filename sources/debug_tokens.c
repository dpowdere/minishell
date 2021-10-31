/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/20 12:51:24 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/31 21:09:07 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	debug_token_iterator(void *data, int ix, int is_last)
{
	t_token	*token;

	(void)ix;
	token = (t_token *)data;
	if (token->type == TOKEN_WORD)
		ft_putstr_fd("word:", STDERR_FILENO);
	else if (token->type == TOKEN_OPERATOR)
		ft_putstr_fd("op:", STDERR_FILENO);
	ft_putstr_fd("'" AEC_YELLOW, STDERR_FILENO);
	ft_putstr_fd(token->string, STDERR_FILENO);
	ft_putstr_fd(AEC_RESET "'", STDERR_FILENO);
	if (!is_last)
		ft_putstr_fd(", ", STDERR_FILENO);
}

inline t_list	*debug_tokens(t_list *tokens_list)
{
	if (DEBUG_TOKENS)
	{
		ft_putstr_fd("TOKENS: [", STDERR_FILENO);
		ft_lstiterix(tokens_list, debug_token_iterator);
		ft_putstr_fd("]\n", STDERR_FILENO);
	}
	return (tokens_list);
}
