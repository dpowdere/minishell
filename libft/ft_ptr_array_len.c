/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ptr_array_len.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/01 21:44:16 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 21:49:09 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

size_t	ft_ptr_array_len(void **ptr_array)
{
	size_t	len;

	len = 0;
	while (*ptr_array++)
		len++;
	return (len);
}
