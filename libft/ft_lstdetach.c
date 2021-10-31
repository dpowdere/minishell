/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstdetach.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/31 13:09:00 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/31 13:14:12 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstdetach(t_list **lst, t_list *detachable_link)
{
	t_list	*cursor;

	if (*lst == detachable_link)
	{
		*lst = NULL;
		return (detachable_link);
	}
	cursor = *lst;
	while (cursor)
	{
		if (cursor->next == detachable_link)
		{
			cursor->next = NULL;
			break ;
		}
		cursor = cursor->next;
	}
	return (detachable_link);
}
