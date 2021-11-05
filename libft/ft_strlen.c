/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 19:59:16 by ngragas           #+#    #+#             */
/*   Updated: 2021/05/03 22:12:20 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

size_t	ft_strlen(const char *s)
{
	const char	*check = s;

	while (check[0])
	{
		if (!check[1])
			return (check - s + 1);
		if (!check[2])
			return (check - s + 2);
		if (!check[3])
			return (check - s + 3);
		if (!check[4])
			return (check - s + 4);
		if (!check[5])
			return (check - s + 5);
		if (!check[6])
			return (check - s + 6);
		if (!check[7])
			return (check - s + 7);
		check += 8;
	}
	return (check - s);
}
