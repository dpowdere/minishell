/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstpipeline.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/01 01:25:20 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/01 01:25:30 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** Stream process list elements.
**
** Each element is reused. Its content can change. Processing of an element can
** yield more than one element, but the first one of them would be the element
** itself. `ft_lststream` can disconnect any number of elements from the chain
** of the list. It basically gets a *list* of elements. `ft_lstpipline` on the
** other hand operates strictly on a single link and gets a single element
** only that is disconnected from the other elements.
**
** `*_xd` versions of the functions make it possible to pass in
** some extra data to the streaming function.
*/

void	ft_lststream(t_list **lst, t_list *(*stream_process)(t_list **))
{
	t_list	*converted_list;

	converted_list = NULL;
	while (*lst)
		ft_lstadd_back(&converted_list, stream_process(lst));
	ft_lstadd_front(lst, converted_list);
}

void	ft_lststream_xd(t_list **lst,
	t_list *(*stream_process)(t_list **, void *), void *extra_data)
{
	t_list	*converted_list;

	converted_list = NULL;
	while (*lst)
		ft_lstadd_back(&converted_list, stream_process(lst, extra_data));
	ft_lstadd_front(lst, converted_list);
}

void	ft_lstpipeline(t_list **lst, t_list *(*pipeline)(t_list *))
{
	t_list	*converted_list;
	t_list	*elem;

	converted_list = NULL;
	while (*lst)
	{
		elem = *lst;
		*lst = (*lst)->next;
		elem->next = NULL;
		ft_lstadd_back(&converted_list, pipeline(elem));
	}
	ft_lstadd_front(lst, converted_list);
}

void	ft_lstpipeline_xd(t_list **lst,
	t_list *(*pipeline)(t_list *, void *), void *extra_data)
{
	t_list	*converted_list;
	t_list	*elem;

	converted_list = NULL;
	while (*lst)
	{
		elem = *lst;
		*lst = (*lst)->next;
		elem->next = NULL;
		ft_lstadd_back(&converted_list, pipeline(elem, extra_data));
	}
	ft_lstadd_front(lst, converted_list);
}
