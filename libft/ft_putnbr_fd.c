/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/18 15:04:54 by ngragas           #+#    #+#             */
/*   Updated: 2020/11/20 20:15:00 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

void	ft_putnbr_fd(int n, int fd)
{
	char			sign;
	unsigned char	i;
	char			digits[11];

	if (!n)
	{
		write(fd, "0", 1);
		return ;
	}
	sign = (char)((n > 0) - (n < 0));
	i = 11;
	while (n)
	{
		digits[--i] = (char)('0' + n % 10 * sign);
		n /= 10;
	}
	if (sign < 0)
		digits[--i] = '-';
	write(fd, &digits[i], 11 - i);
}
