/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cook_star.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 06:56:07 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/05 06:56:10 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <dirent.h>
#include <sys/types.h>

#include "minishell.h"

// TODO: Sort star expanded pathnames in lexicographical order like in bash,
// not just in ASCII order.
static t_list	*get_star_list(void)
{
	DIR				*dp;
	struct dirent	*dir_entry;
	t_list			*lst;

	lst = NULL;
	dp = opendir(".");
	dir_entry = readdir(dp);
	while (dir_entry)
	{
		if (ft_strncmp(".", dir_entry->d_name, 1))
			ft_lstadd_front(&lst, ft_lstnew(dir_entry->d_name));
		dir_entry = readdir(dp);
	}
	ft_lstsort(&lst, ft_strcmp);
	return (lst);
}

static void	*convert_to_wordparts(void *data)
{
	return (lstnew_wordpart((char *)data, FINAL));
}

static void	*gather_next_phases(void *initial, void *next)
{
	t_part	*part;

	part = (t_part *)next;
	part->phase = next_phase(part->phase);
	*(int *)initial += part->phase;
	return (initial);
}

void	cook_wordpart_expand_pathnames(t_cc *cc)
{
	int		should_expand;
	t_list	*star_list;

	should_expand = true;
	ft_lstreduce(cc->part_list, &should_expand, should_expand_pathnames_check);
	if (should_expand)
	{
		star_list = get_star_list();
		if (star_list != NULL)
		{
			star_list = ft_lstconv(star_list, convert_to_wordparts);
			ft_lstadd_back(&star_list, cc->word_list->next);
			ft_lstclear((t_list **)&cc->word_list->content, free);
			cc->word_list->content = star_list->content;
			cc->word_list->next = star_list->next;
			free(star_list);
			cc->part_list = cc->word_list->content;
			cc->recycle_wordpart = true;
			return ;
		}
	}
	ft_lstreduce(cc->part_list,
		&cc->need_another_traversal, gather_next_phases);
	cc->dont_change_phase = true;
	cc->part_list = ft_lstlast(cc->part_list);
}

void	*should_expand_pathnames_check(void *initial, void *next)
{
	t_part	*part;
	char	*cursor;
	int		valid_phase;
	int		stars_only;

	part = (t_part *)next;
	valid_phase = (part->phase == PATHNAME_EXPANSION \
					|| part->phase == _PATHEXP_IN_VARIABLE);
	cursor = part->start;
	stars_only = *cursor == '*';
	while (stars_only && *cursor != '\0' && cursor < part->exclusive_end)
		stars_only *= (*cursor++ == '*');
	*(int *)initial *= valid_phase * stars_only;
	return (initial);
}
