/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstiter.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/20 21:06:03 by ngragas           #+#    #+#             */
/*   Updated: 2020/11/20 21:07:44 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstiter(t_list *lst, void (*f)(void *))
{
	while (lst)
	{
		f(lst->content);
		lst = lst->next;
	}
}

void	ft_lstiterix(t_list *lst, void (*f)(void *data, int ix, int is_last))
{
	int	ix;

	ix = 0;
	while (lst)
	{
		f(lst->content, ++ix, lst->next == NULL);
		lst = lst->next;
	}
}
