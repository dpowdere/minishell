/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:38:40 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/18 18:43:15 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	check_sequence(t_token *token, t_token *prev_token)
{
	if (token->type == TOKEN_WORD \
	&& prev_token->type == TOKEN_OPERATOR && *prev_token->string == ')')
		return (false);
	else if (token->type == TOKEN_OPERATOR)
	{
		if (prev_token->type == TOKEN_WORD && *token->string == '(')
			return (false);
		else if (prev_token->type == TOKEN_OPERATOR)
		{
			if (*token->string == '(' && ft_strchr(")<>", *prev_token->string))
				return (false);
			else if (*token->string != '(' && *prev_token->string != ')')
				return (false);
			else if (ft_strchr("<>", *token->string) \
			&& ft_strchr("<>", *prev_token->string))
				return (false);
		}
	}
	return (true);
}

static char	*check_tokens_sequence(const t_list *tokens_list)
{
	t_token	*token;
	t_token	*prev_token;
	bool	words_started;

	prev_token = NULL;
	words_started = false;
	while (tokens_list)
	{
		token = tokens_list->content;
		if (token->type == TOKEN_WORD)
			words_started = true;
		else if (token->type == TOKEN_OPERATOR && *token->string == '(')
			words_started = false;
		else if (token->type == TOKEN_OPERATOR \
		&& ft_strchr("|&)", *token->string) && words_started == false)
			return (token->string);
		if (prev_token && check_sequence(token, prev_token) == false)
			return (token->string);
		prev_token = token;
		tokens_list = tokens_list->next;
	}
	if (prev_token->type == TOKEN_OPERATOR && *prev_token->string != ')')
		return (prev_token->string);
	return (NULL);
}

static char	*check_brackets_count(const t_list *tokens_list)
{
	t_token	*token;
	int		opened_brackets;

	opened_brackets = 0;
	while (tokens_list)
	{
		token = tokens_list->content;
		if (token->type == TOKEN_OPERATOR)
		{
			if (*token->string == '(')
				opened_brackets++;
			else if (*token->string == ')')
				opened_brackets--;
		}
		if (opened_brackets < 0)
			break ;
		tokens_list = tokens_list->next;
	}
	if (opened_brackets)
		return (token->string);
	return (NULL);
}

bool	check_tokens(t_list *tokens_list)
{
	char	*(*checkers[2])(const t_list *);
	char	*error_token;
	int		i;

	if (tokens_list == NULL)
		return (true);
	checkers[0] = check_brackets_count;
	checkers[1] = check_tokens_sequence;
	i = 0;
	while (i < 2)
	{
		error_token = checkers[i](tokens_list);
		if (error_token)
		{
			if (i == 0)
				error(ERR_SYNTAX_MATCHING, ")", tokens_list, free_token);
			else if (i == 1)
				error(ERR_SYNTAX_TOKEN, error_token, tokens_list, free_token);
			errno = ERR_CODE_PARSE;
			return (false);
		}
		i++;
	}
	return (true);
}
