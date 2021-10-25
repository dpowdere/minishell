/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstinsert.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/25 21:54:19 by dpowdere          #+#    #+#             */
/*   Updated: 2021/10/25 21:54:26 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstinsert(t_list **current, t_list *new_next)
{
	if (current == NULL)
		return ;
	if (*current == NULL)
		*current = new_next;
	else
	{
		ft_lstadd_back(&new_next, (*current)->next);
		(*current)->next = new_next;
	}
}
