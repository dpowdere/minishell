/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 20:01:22 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/15 21:50:57 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// TODO: move to libft
int	ft_ptr_array_len(const void **ptr_array)
{
	int	len;

	len = 0;
	while (*ptr_array++)
		len++;
	return (len);
}

void	ft_putendl_fd_array(char **str_array, int fd)
{
	while (*str_array)
		ft_putendl_fd(*str_array++, fd);
}

char	*ft_strjoin_chr(char const *s1, char const *s2, char c)
{
	char	*str;
	char	*cur;
	size_t	len1;
	size_t	len2;

	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	str = malloc(len1 + len2 + 2);
	if (str == NULL)
		return (NULL);
	cur = str;
	while (len1--)
		*cur++ = *s1++;
	*cur++ = c;
	while (len2--)
		*cur++ = *s2++;
	*cur = '\0';
	return (str);
}
