/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_tokens.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 18:38:40 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/31 21:09:07 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define METACHARACTERS " \t\n|<>()"
#define OPERATOR_CHARS "|<>()"

static bool	add_token(
		enum e_type type, const char *src, size_t len, t_list **tokens_list)
{
	char	*string;
	t_token	*token;
	t_list	*tokenlst;

	string = ft_substr(src, 0, len);
	if (string == NULL)
		return (false);
	token = malloc(sizeof(*token));
	if (token == NULL)
	{
		free(string);
		return (false);
	}
	token->type = type;
	token->string = string;
	tokenlst = ft_lstnew(token);
	if (tokenlst == NULL)
	{
		free(string);
		free(token);
		return (false);
	}
	ft_lstadd_front(tokens_list, tokenlst);
	return (true);
}

static const char	*get_word(
		const char *line, t_list **tokens_list, char *error_unclosed)
{
	const char	*word_start = line;
	bool		opened_single_quote;
	bool		opened_double_quote;

	opened_single_quote = false;
	opened_double_quote = false;
	while (*line && (opened_double_quote || opened_single_quote \
	|| (!ft_strchr(METACHARACTERS, *line) && ft_memcmp(line, "&&", 2) != 0)))
	{
		if (*line == '\'' && !opened_double_quote)
			opened_single_quote = !opened_single_quote;
		else if (*line == '\"' && !opened_single_quote)
			opened_double_quote = !opened_double_quote;
		else if (*line == '\\' && !opened_single_quote && *(line + 1) != '\0')
			line++;
		line++;
	}
	if (opened_single_quote)
		*error_unclosed = '\'';
	else if (opened_double_quote)
		*error_unclosed = '\"';
	if (opened_single_quote || opened_double_quote \
	|| !add_token(TOKEN_WORD, word_start, line - word_start, tokens_list))
		return (NULL);
	return (line);
}

static const char	*get_operator(const char *line, t_list **tokens_list)
{
	int	operator_len;

	if (!ft_memcmp(line, "||", 2) || !ft_memcmp(line, "&&", 2) \
		|| !ft_memcmp(line, "<<", 2) || !ft_memcmp(line, ">>", 2))
		operator_len = 2;
	else
		operator_len = 1;
	if (add_token(TOKEN_OPERATOR, line, operator_len, tokens_list) == false)
		return (NULL);
	return (line + operator_len);
}

t_list	*get_tokens_list(const char *line, int *exit_status)
{
	t_list	*tokens_list;
	char	error_unclosed[2];

	tokens_list = NULL;
	ft_bzero(error_unclosed, 2);
	while (line && *line)
	{
		while (ft_isspace(*line))
			line++;
		if (*line == '\0')
			break ;
		if (ft_strchr(OPERATOR_CHARS, *line) || !ft_memcmp(line, "&&", 2))
			line = get_operator(line, &tokens_list);
		else
			line = get_word(line, &tokens_list, error_unclosed);
	}
	if (*error_unclosed && errno != ENOMEM)
	{
		error(ERR_SYNTAX_MATCHING, error_unclosed, NULL, NULL);
		*exit_status = ERR_CODE_PARSE;
		return (error(ERR_SYNTAX_EOF, NULL, tokens_list, free_token));
	}
	else if (line == NULL)
		return (error(strerror(errno), NULL, tokens_list, free_token));
	return (debug_tokens(ft_lstreverse(&tokens_list)));
}

t_list	*get_tokens_list_subshell(char **tokens)
{
	t_list	*tokens_list;
	char	_;

	tokens_list = NULL;
	while (*tokens)
	{
		if (ft_strchr(OPERATOR_CHARS, **tokens) || !ft_memcmp(*tokens, "&&", 2))
			get_operator(*tokens, &tokens_list);
		else
			get_word(*tokens, &tokens_list, &_);
		if (errno == ENOMEM)
			return (error(strerror(errno), NULL, tokens_list, free_token));
		tokens++;
	}
	return (debug_tokens(ft_lstreverse(&tokens_list)));
}
