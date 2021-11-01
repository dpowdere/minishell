/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 18:40:16 by ngragas           #+#    #+#             */
/*   Updated: 2021/10/16 15:06:07 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;

char	**copy_environ(bool deep_copy)
{
	const int	env_var_count = ft_ptr_array_len((const void **)environ);
	char		**new_env;
	int			i;

	new_env = malloc((env_var_count + 1 + 1) * sizeof(*new_env));
	if (new_env == NULL)
		return (NULL);
	if (deep_copy)
	{
		i = 0;
		while (i < env_var_count)
		{
			new_env[i] = ft_strdup(environ[i]);
			if (new_env[i] == NULL)
				return (ft_free_ptr_array((void **)new_env));
			i++;
		}
	}
	else
		new_env = ft_memcpy(new_env, environ, env_var_count * sizeof(*environ));
	new_env[env_var_count] = NULL;
	new_env[env_var_count + 1] = NULL;
	return (new_env);
}

static int	extend_environ(char *env_var)
{
	const int	env_var_count = ft_ptr_array_len((const void **)environ);
	char		**new_environ;

	if (env_var == NULL)
		return (-1);
	new_environ = copy_environ(ENV_DEEP_COPY_FALSE);
	if (new_environ == NULL)
	{
		free(env_var);
		return (-1);
	}
	new_environ[env_var_count] = env_var;
	free(environ);
	environ = new_environ;
	return (0);
}

static char	**get_env_ptr(const char *name)
{
	char	**cur_env;
	int		cur_name_len;

	cur_env = environ;
	while (*cur_env)
	{
		cur_name_len = ft_strchr(*cur_env, '=') - *cur_env;
		if (ft_strncmp(*cur_env, name, cur_name_len) == 0)
			return (cur_env);
		cur_env++;
	}
	return (NULL);
}

int	set_env(const char *name, const char *value)
{
	char	**cur_env;

	if (name == NULL || *name == '\0' || ft_strchr(name, '='))
	{
		errno = EINVAL;
		return (-1);
	}
	cur_env = get_env_ptr(name);
	if (cur_env == NULL)
		return (extend_environ(ft_strjoin_chr(name, value, '=')));
	free(*cur_env);
	*cur_env = ft_strjoin_chr(name, value, '=');
	if (*cur_env == NULL)
		return (-1);
	return (0);
}

int	unset_env(const char *name)
{
	char	**cur_env;
	char	**next_env;

	if (name == NULL || *name == '\0' || ft_strchr(name, '='))
	{
		errno = EINVAL;
		return (-1);
	}
	cur_env = get_env_ptr(name);
	if (cur_env)
	{
		next_env = cur_env + 1;
		free(*cur_env);
		while (*cur_env)
			*cur_env++ = *next_env++;
	}
	return (0);
}
