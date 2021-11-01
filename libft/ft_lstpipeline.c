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
** itself. `ft_lstpipeline` can disconnect any number of elements from the chain
** of the list, as it gets a list of elements. `ft_lstpipline1` operates
** strictly on a single link, as it gets one element only disconnected from
** the other elements.
**
** `*_extradata` versions of the functions make it possible to pass in
** some extra data to the streaming function.
*/

void	ft_lstpipeline(t_list **lst, t_list *(*pipeline)(t_list **))
{
	t_list	*converted_list;

	converted_list = NULL;
	while (*lst)
		ft_lstadd_back(&converted_list, pipeline(lst));
	ft_lstadd_front(lst, converted_list);
}

void	ft_lstpipeline_extradata(t_list **lst,
	t_list *(*pipeline)(t_list **, void *), void *extra_data)
{
	t_list	*converted_list;

	converted_list = NULL;
	while (*lst)
		ft_lstadd_back(&converted_list, pipeline(lst, extra_data));
	ft_lstadd_front(lst, converted_list);
}

void	ft_lstpipeline1(t_list **lst, t_list *(*pipeline)(t_list *))
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

void	ft_lstpipeline1_extradata(t_list **lst,
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
