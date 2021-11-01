/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/09 12:48:22 by ngragas           #+#    #+#             */
/*   Updated: 2020/11/12 19:27:09 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

char	*ft_strrchr(const char *s, int c)
{
	const char	*res;

	if (*s == (char)c)
		res = s;
	else
		res = NULL;
	while (*s)
		if (*++s == (char)c)
			res = s;
	return ((char *)res);
}
