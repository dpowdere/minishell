/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpowdere <dpowdere@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/04 22:53:28 by dpowdere          #+#    #+#             */
/*   Updated: 2021/11/04 22:54:54 by dpowdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	*calc_heredoc_len(void *initial, void *next)
{
	*(size_t *)initial += ft_strlen(next) + 1;
	return (initial);
}

void	*populate_heredoc(void *initial, void *next)
{
	size_t		len;

	len = ft_strlen((char *)next);
	*((char *)initial - 1) = '\n';
	initial = (char *)initial - len - 1;
	ft_memcpy(initial, (char *)next, len);
	free(next);
	return (initial);
}

void	input_heredoc(t_cmd *cmd, const char *terminator)
{
	t_list	*lst;
	char	*line;
	size_t	len;

	lst = NULL;
	free(cmd->heredoc);
	while (true)
	{
		line = readline(HEREDOC_PROMPT_STRING);
		if (!line || ft_strncmp(line, terminator, ft_strlen(line)) == 0)
		{
			if (line == NULL)
			{
				ft_putstr_fd("warning: here-document delimited ", 2);
				ft_putstr_fd("by end-of-file (wanted `", STDERR_FILENO);
				ft_putstr_fd((char *)terminator, STDERR_FILENO);
				ft_putendl_fd("')", STDERR_FILENO);
			}
			len = 1;
			ft_lstreduce(lst, &len, calc_heredoc_len);
			cmd->heredoc = malloc(len);
			if (cmd->heredoc == NULL)
			{
				ft_lstclear(&lst, free);
				break ;
			}
			*(cmd->heredoc + len - 1) = '\0';
			ft_lstpopreduce(&lst, cmd->heredoc + len - 1, populate_heredoc);
			break ;
		}
		else
			ft_lstadd_front(&lst, ft_lstnew(line));
	}
}

t_list	*cook_heredoc_terminator(t_list *lst, t_cmd *cmd,
									t_redirect *redirect, int *exit_status)
{
	char	*raw_terminator;
	t_cc	cc;

	if (DEBUG_CMD_COOKING)
		debug_redirect(redirect);
	lst->content = lstnew_wordpart(redirect->target, QUOTE_REMOVAL);
	if (lst->content == NULL)
	{
		lst->content = redirect;
		return (lst);
	}
	raw_terminator = ft_strdup(redirect->target);
	if (raw_terminator == NULL)
	{
		free(lst->content);
		lst->content = redirect;
		return (lst);
	}
	cc = get_cooking_cursor(lst, exit_status);
	while (wordpart_cooking_condition(&cc))
	{
		cook_wordpart(&cc);
		next_wordpart(&cc, lst);
	}
	cmd->heredoc_expand_vars = !ft_strcmp(redirect->target, raw_terminator);
	if (DEBUG_CMD_COOKING)
		printf("\n\n");
	free(ft_lstpop((t_list **)&lst->content));
	free(raw_terminator);
	lst->content = redirect;
	return (lst);
}

t_list	*filter_heredoc(t_list *lst, void *extra_data)
{
	t_redirect	*redirect;
	t_xd		*xd;

	redirect = lst->content;
	xd = extra_data;
	if (redirect->type == REDIRECT_IN_HEREDOC)
	{
		cook_heredoc_terminator(lst, xd->cmd, redirect, xd->exit_status);
		input_heredoc(xd->cmd, redirect->target);
		free_redirect(ft_lstpop(&lst));
	}
	return (lst);
}

void	*input_heredocs(void *cmd_data, void *exit_status)
{
	t_xd	xd;

	xd.cmd = cmd_data;
	xd.exit_status = exit_status;
	ft_lstpipeline_xd(&xd.cmd->redirects, filter_heredoc, &xd);
	return (cmd_data);
}
