/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 12:45:14 by emgenc            #+#    #+#             */
/*   Updated: 2025/03/17 11:02:54 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include "../t_split_utils/t_split.h"

// FUNCTION PROTOTYPE FORWARD DECLARATION: IS THIS ALLOWED?
// I THINK IT IS ALLOWED, BUT I AM NOT SURE.
void	*env(char **envp);

static char	**reallocenv(char ***old_envp, char **new_var, int *count)
{
	char	**new_envp;
	int		i;

	new_envp = malloc(sizeof(char *) * ((*count) + 2));
	if (!new_envp)
		return (perror("malloc"), free((*new_var)), NULL);
	i = -1;
	while (++i < *count)
		new_envp[i] = (*old_envp)[i];
	new_envp[*count] = *new_var;
	new_envp[(*count) + 1] = NULL;
	return (new_envp);
}

int	export(char *to_be_exported, char ***envp)
{
	char	*equal_sign_pos;
	char	*new_var;
	int		i;
	size_t	var_len;
	char	**new_envp;

	equal_sign_pos = ft_strchr(to_be_exported, '=');
	if (!equal_sign_pos)
		return (env(*envp), 1);
	new_var = ft_strdup(to_be_exported);
	if (!new_var)
		return (perror("malloc"), 1);
	i = -1;
	var_len = equal_sign_pos - to_be_exported;
	while ((*envp)[++i])
	{
		if (ft_strncmp((*envp)[i], to_be_exported, var_len) == 0
			&& (*envp)[i][var_len] == '=')
			return (free((*envp)[i]), (*envp)[i] = new_var, 0);
	}
	new_envp = reallocenv(envp, &new_var, &i);
	if (!new_envp)
		return (1);
	return (free(*envp), *envp = new_envp, 0);
}
