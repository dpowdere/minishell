/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/09 13:09:30 by ngragas           #+#    #+#             */
/*   Updated: 2021/02/03 16:00:31 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	needle_len;

	if (!*little)
		return ((char *)big);
	needle_len = ft_strlen(little);
	if (!len || (*big && len < needle_len))
		return (NULL);
	len -= needle_len - 1;
	while (len && *big)
	{
		if (*big == *little)
			if (!ft_strncmp(big + 1, little + 1, needle_len - 1))
				return ((char *)big);
		big++;
		len--;
	}
	return (NULL);
}
