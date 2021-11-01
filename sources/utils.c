/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 20:01:22 by ngragas           #+#    #+#             */
/*   Updated: 2021/11/01 20:08:09 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// TODO: move to libft
int	ft_ptr_array_len(void **ptr_array)
{
	int	len;

	len = 0;
	while (*ptr_array++)
		len++;
	return (len);
}

void	**ft_lst_to_ptr_array(t_list *list)
{
	int		elements;
	void	**ptr_array;
	int		i;

	elements = ft_lstsize(list);
	ptr_array = malloc((elements + 1) * sizeof(*ptr_array));
	if (ptr_array == NULL)
		return (NULL);
	i = 0;
	while (list)
		ptr_array[i++] = ft_lstpop(&list);
	ptr_array[i] = NULL;
	return (ptr_array);
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

int	ft_isspace(int c)
{
	c = (unsigned char)c;
	return (c == ' ' || c == '\n' || c == '\t' || \
			c == '\v' || c == '\f' || c == '\r');
}

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

int	error_builtin(char *builtin_name, char *message, char *extra_message)
{
	ft_putstr_fd(COMMAND_NAME ": ", STDERR_FILENO);
	ft_putstr_fd(builtin_name, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putstr_fd(message, STDERR_FILENO);
	if (extra_message)
	{
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(extra_message, STDERR_FILENO);
	}
	ft_putstr_fd("\n", STDERR_FILENO);
	return (EXIT_FAILURE);
}

void	*error(enum e_error type, char *extra_message,
				t_list *list_to_free, void (*free_fn)(void*))
{
	ft_putstr_fd(COMMAND_NAME ": ", STDERR_FILENO);
	if (type == ERR_ERRNO)
		ft_putstr_fd(strerror(errno), STDERR_FILENO);
	else if (type == ERR_SYNTAX_EOF)
		ft_putstr_fd(ERR_STR_SYNTAX_EOF, STDERR_FILENO);
	else if (type == ERR_SYNTAX_MATCHING)
		ft_putstr_fd(ERR_STR_SYNTAX_MATCHING, STDERR_FILENO);
	else if (type == ERR_SYNTAX_TOKEN)
		ft_putstr_fd(ERR_STR_SYNTAX_TOKEN, STDERR_FILENO);
	else if (type == ERR_COMMAND_NOT_FOUND)
		ft_putstr_fd(ERR_STR_COMMAND_NOT_FOUND, STDERR_FILENO);
	if (extra_message)
	{
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(extra_message, STDERR_FILENO);
	}
	ft_putstr_fd("\n", STDERR_FILENO);
	if (list_to_free && free_fn)
		ft_lstclear(&list_to_free, free_fn);
	return (NULL);
}

void	*exit_with_error(enum e_error type, char *extra_message,
						t_list *list_to_free, void (*free_fn)(void*))
{
	error(type, extra_message, list_to_free, free_fn);
	exit(errno);
}

int	pid_comparator(const pid_t *pid, const pid_t *pid_to_find)
{
	return (*pid == *pid_to_find);
}

bool	valid_identifier_name(const char *name)
{
	if (name == NULL || name[0] == '\0' || ft_isdigit(name[0]))
		return (false);
	while (*name)
	{
		if (ft_isalnum(*name) || *name == '_')
			name++;
		else
			return (false);
	}
	return (true);
}
