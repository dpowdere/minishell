/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstpop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/23 22:52:09 by ngragas           #+#    #+#             */
/*   Updated: 2021/02/23 22:57:28 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_lstpop(t_list **lst)
{
	void	*data;
	t_list	*cur;

	data = (*lst)->content;
	cur = *lst;
	*lst = (*lst)->next;
	free(cur);
	return (data);
}

t_list	*ft_lstpopconv(t_list **lst, void *(*f)(void *))
{
	t_list	*converted_item;

	converted_item = *lst;
	if (converted_item)
	{
		if (f)
			converted_item->content = f((*lst)->content);
		*lst = (*lst)->next;
		converted_item->next = NULL;
	}
	return (converted_item);
}

t_list	*ft_lstpopconv_xd(t_list **lst,
							void *(*f)(void *, void *), void *extra_data)
{
	t_list	*converted_item;

	converted_item = *lst;
	if (converted_item)
	{
		converted_item->content = f((*lst)->content, extra_data);
		*lst = (*lst)->next;
		converted_item->next = NULL;
	}
	return (converted_item);
}
