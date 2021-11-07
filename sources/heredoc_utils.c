/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 22:02:47 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/05 22:03:08 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	*calc_heredoc_len(void *initial, void *next)
{
	*(size_t *)initial += ft_strlen(next) + 1;
	return (initial);
}

void	*populate_heredoc(void *initial, void *next)
{
	const size_t	len = ft_strlen((char *)next);

	*((char *)initial - 1) = '\n';
	initial = (char *)initial - len - 1;
	ft_memcpy(initial, (char *)next, len);
	free(next);
	return (initial);
}

void	populate_heredoc_from_lines(t_cmd *cmd, t_list *lst)
{
	size_t	len;

	len = 1;
	ft_lstreduce(lst, &len, calc_heredoc_len);
	cmd->heredoc = malloc(len);
	if (cmd->heredoc == NULL)
		return ((void)ft_lstclear(&lst, free));
	*(cmd->heredoc + len - 1) = '\0';
	ft_lstpopreduce(&lst, cmd->heredoc + len - 1, populate_heredoc);
}
