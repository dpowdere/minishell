/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstreduce.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/26 02:27:13 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/26 02:32:03 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_lstreduce(t_list *lst, void *initial, void *(*f)(void *, void *))
{
	while (lst)
	{
		initial = f(initial, lst->content);
		lst = lst->next;
	}
	return (initial);
}

void	*ft_lstpopreduce(t_list **lst, void *init, void *(*f)(void *, void *))
{
	while (*lst)
		init = f(init, ft_lstpop(lst));
	return (init);
}
