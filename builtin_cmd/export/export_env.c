/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 00:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/25 11:17:37 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static char	**allocate_new_envp(t_shell *shell)
{
	char	**new_envp;
	int		i;

	i = 0;
	while (shell->envp[i])
		i++;
	new_envp = malloc(sizeof(char *) * (i + 2));
	if (!new_envp)
		return (NULL);
	ft_memcpy(new_envp, shell->envp, sizeof(char *) * i);
	return (new_envp);
}

static char	*create_env_string(char *var_name, char *value)
{
	char	*temp;
	char	*result;

	temp = ft_strjoin(var_name, "=");
	if (!temp)
		return (NULL);
	result = ft_strjoin(temp, value);
	free(temp);
	return (result);
}

static void	finalize_new_envp(char **new_envp, char *new_var, t_shell *shell)
{
	int	i;

	i = 0;
	while (shell->envp[i])
		i++;
	new_envp[i] = new_var;
	new_envp[i + 1] = NULL;
	free(shell->envp);
	shell->envp = new_envp;
}

int	new_env_var(char *var_name, char *value, t_shell *shell)
{
	char	**new_envp;
	char	*new_var;

	new_envp = allocate_new_envp(shell);
	if (!new_envp)
		return (1);
	new_var = create_env_string(var_name, value);
	if (!new_var)
	{
		free(new_envp);
		return (1);
	}
	finalize_new_envp(new_envp, new_var, shell);
	return (0);
}

int	new_empty_envvar(char *var_name, t_shell *shell)
{
	char	**new_envp;
	char	*new_var;

	new_envp = allocate_new_envp(shell);
	if (!new_envp)
		return (1);
	new_var = ft_strdup(var_name);
	if (!new_var)
	{
		free(new_envp);
		return (1);
	}
	finalize_new_envp(new_envp, new_var, shell);
	return (0);
}
