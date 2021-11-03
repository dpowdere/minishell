/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_write.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/02 20:42:42 by dpowdere          #+#    #+#             */
/*   Updated: 2021/09/30 22:29:47 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <unistd.h>

/*
** An iterative equivalent of the `write` system call, returning
** the value of the last call, if an error occurs, and the total
** number of bytes written if every call to `write` is successful.
*/

ssize_t	ft_write(int fd, const void *buf, size_t size)
{
	ssize_t	total_size;
	ssize_t	partial_size;

	total_size = 0;
	partial_size = write(fd, buf, size);
	while (partial_size >= 0 && (size_t)partial_size < size)
	{
		total_size += partial_size;
		buf = (unsigned char *)buf + partial_size;
		size -= partial_size;
		partial_size = write(fd, buf, size);
	}
	if (partial_size < 0)
		return (partial_size);
	total_size += partial_size;
	return (total_size);
}
