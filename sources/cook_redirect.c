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

t_list	*cook_redirect(t_list *lst, void *exit_status)
{
	t_redirect	*redirect;
	size_t		len;
	char		*s1;
	char		*s2;
	t_cc		cc;

	redirect = lst->content;
	if (DEBUG_CMD_COOKING)
		debug_redirect(redirect);
	lst->content = lstnew_wordpart(redirect->target, VARIABLE_SUBSTITUTION);
	if (lst->content == NULL)
	{
		lst->content = redirect;
		return (lst);
	}
	s1 = redirect->target;
	s2 = ft_strdup(s1);
	if (s2 == NULL)
		s2 = s1;
	cc = get_cooking_cursor(lst, exit_status);
	while (wordpart_cooking_condition(&cc))
	{
		cook_wordpart(&cc);
		next_wordpart(&cc, lst);
	}
	len = 0;
	if (ft_lstsize(lst) > 1 || *(size_t *)ft_lstreduce(
			(t_list *)lst->content, &len, calc_memsize) == 0)
	{
		ft_lstclear(&lst, free_word_with_parts);
		free(redirect);
		error(s2, ERR_AMBIGUOUS_REDIRECT, NULL, NULL);
		*(int *)exit_status = 1;
	}
	else
	{
		ft_lstconv(lst, serve);
		if (DEBUG_CMD_COOKING)
			printf("\n\n");
		redirect->target = lst->content;
		lst->content = redirect;
	}
	free(s1);
	if (s2 != s1)
		free(s2);
	return (lst);
}
