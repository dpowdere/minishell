/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cook_serve.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 08:24:38 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/05 08:24:40 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	*calc_memsize(void *initial, void *next)
{
	t_part	*part;

	part = (t_part *)next;
	*(size_t *)initial += part->exclusive_end - part->start;
	return (initial);
}

void	*populate_arg(void *initial, void *next)
{
	t_part		*part;
	ptrdiff_t	memsize;

	part = (t_part *)next;
	memsize = part->exclusive_end - part->start;
	initial = (char *)initial - memsize;
	ft_memcpy(initial, part->start, memsize);
	free(part);
	return (initial);
}

void	no_free(void *data)
{
	(void)data;
}

// Convert word parts into final words
void	*serve(void *data)
{
	t_list	*part_list;
	size_t	memsize;
	char	*string_terminator;

	part_list = (t_list *)data;
	memsize = 1;
	ft_lstreduce(part_list, &memsize, calc_memsize);
	string_terminator = malloc(memsize);
	if (string_terminator == NULL)
	{
		ft_lstclear(&part_list, no_free);
		return (NULL);
	}
	part_list = ft_lstreverse(&part_list);
	string_terminator += memsize - 1;
	*string_terminator = '\0';
	return (debug_cooked_string(
			ft_lstpopreduce(&part_list, string_terminator, populate_arg)));
}
