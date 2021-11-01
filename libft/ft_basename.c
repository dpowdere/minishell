/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_basename.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/01 22:45:09 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 22:46:28 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <errno.h>
#include <limits.h>

char	*ft_basename(char *path)
{
	static char	basename_bss[PATH_MAX];
	size_t		end_index;
	size_t		start_index;

	if (path == NULL || *path == '\0')
	{
		basename_bss[0] = '.';
		basename_bss[1] = '\0';
		return (basename_bss);
	}
	end_index = ft_strlen(path);
	while (end_index > 1 && path[end_index - 1] == '/')
		end_index--;
	start_index = end_index - 1;
	while (start_index > 0 && path[start_index - 1] != '/')
		start_index--;
	if (end_index - start_index >= PATH_MAX)
	{
		errno = ENAMETOOLONG;
		return (NULL);
	}
	ft_memcpy(basename_bss, path + start_index, end_index - start_index);
	basename_bss[end_index - start_index] = '\0';
	return (basename_bss);
}
