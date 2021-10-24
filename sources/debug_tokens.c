/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/20 12:51:24 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/20 12:51:31 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	debug_token_iterator(void *data, int ix, int is_last)
{
	t_token	*token;

	(void)ix;
	token = (t_token *)data;
	if (token->type == TOKEN_WORD)
		printf("word:");
	else if (token->type == TOKEN_OPERATOR)
		printf("op:");
	printf("'" AEC_YELLOW "%s" AEC_RESET "'", token->string);
	if (!is_last)
		printf(", ");
}

inline t_list	*debug_tokens(t_list *tokens_list)
{
	if (DEBUG_TOKENS)
	{
		printf(AEC_BOLD "TOKENS" AEC_RESET ": [");
		ft_lstiterix(tokens_list, debug_token_iterator);
		printf("]\n");
	}
	return (tokens_list);
}
