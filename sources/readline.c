/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 18:41:30 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/11 18:44:16 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*read_user_line(void)
{
	char	*line;

	line = readline(PROMPT_STRING "$: ");
	if (line && *line)
		add_history(line);
	return (line);
}
