/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lst_to_ptr_array.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/01 22:40:54 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 22:40:54 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	**ft_lst_to_ptr_array(t_list **list)
{
	int		elements;
	void	**ptr_array;
	int		i;

	elements = ft_lstsize(*list);
	ptr_array = malloc((elements + 1) * sizeof(*ptr_array));
	if (ptr_array == NULL)
		return (NULL);
	i = 0;
	while (*list)
		ptr_array[i++] = ft_lstpop(list);
	ptr_array[i] = NULL;
	return (ptr_array);
}
