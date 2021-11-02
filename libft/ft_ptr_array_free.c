/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ptr_array_free.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/02 17:12:33 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 21:57:36 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

void	*ft_ptr_array_free(void **array)
{
	unsigned int	i;

	if (array)
	{
		i = 0;
		while (array[i])
			free(array[i++]);
		free(array);
	}
	return (NULL);
}

void	*ft_ptr_array_nfree(void **array, size_t count)
{
	size_t	i;

	i = 0;
	while (i < count)
		free(array[i++]);
	free(array);
	return (NULL);
}
