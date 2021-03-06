/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cook_redirect.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 08:27:42 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/05 08:27:44 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_word_with_parts(void *data)
{
	ft_lstclear((t_list **)&data, free);
}

static void	debug_output(t_list *lst)
{
	if (DEBUG_CMD_COOKING)
	{
		if (lst == NULL)
			debug_cooked_string(NULL);
		printf("\n\n");
	}
}

static void	cook_redirect_cleanup(t_list **lst, t_redirect *redirect,
									char *raw_dst, int *exit_status)
{
	size_t	len;
	int		lstsize;

	len = 0;
	lstsize = ft_lstsize(*lst);
	debug_output(*lst);
	if (lstsize != 1 || *(size_t *)ft_lstreduce(
			(t_list *)(*lst)->content, &len, calc_memsize) == 0)
	{
		ft_lstclear(lst, free_word_with_parts);
		free(redirect);
		if (lstsize == 1)
			error("", strerror(ENOENT), NULL, NULL);
		else
			error(raw_dst, ERR_AMBIGUOUS_REDIRECT, NULL, NULL);
		*(int *)exit_status = 1;
	}
	else
	{
		*lst = ft_lstconv(*lst, serve);
		redirect->target = (*lst)->content;
		(*lst)->content = redirect;
	}
}

t_list	*cook_redirect(t_list *lst, void *exit_status)
{
	t_redirect	*redirect;
	char		*cooked_dst;
	char		*raw_dst;

	redirect = lst->content;
	if (DEBUG_CMD_COOKING)
		debug_redirect(redirect);
	lst->content = lstnew_wordpart(redirect->target, VARIABLE_SUBSTITUTION);
	if (lst->content == NULL)
	{
		lst->content = redirect;
		return (lst);
	}
	cooked_dst = redirect->target;
	raw_dst = ft_strdup(cooked_dst);
	if (raw_dst == NULL)
		raw_dst = cooked_dst;
	cook(&lst, exit_status);
	cook_redirect_cleanup(&lst, redirect, raw_dst, exit_status);
	free(cooked_dst);
	if (raw_dst != cooked_dst)
		free(raw_dst);
	return (lst);
}
