/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 00:00:00 by user              #+#    #+#             */
/*   Updated: 2025/07/20 19:57:43 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_environment(t_shell *shell, char **envp)
{
	int	i;
	int	count;

	count = 0;
	while (envp[count])
		count++;
	shell->envp = malloc(sizeof(char *) * (count + 1));
	if (!shell->envp)
		return ;
	i = 0;
	while (i < count)
	{
		shell->envp[i] = ft_strdup(envp[i]);
		if (!shell->envp[i])
		{
			while (--i >= 0)
				free(shell->envp[i]);
			free(shell->envp);
			shell->envp = NULL;
			return ;
		}
		i++;
	}
	shell->envp[i] = NULL;
}

void	free_environment(t_shell *shell)
{
	int	i;

	if (!shell->envp)
		return ;
	i = 0;
	while (shell->envp[i])
	{
		free(shell->envp[i]);
		i++;
	}
	free(shell->envp);
	shell->envp = NULL;
}

char	*get_env_value(char *var_name, t_shell *shell)
{
	int		i;
	int		len;
	char	*eq_pos;

	if (!var_name || !shell->envp)
		return (NULL);
	len = ft_strlen(var_name);
	i = 0;
	while (shell->envp[i])
	{
		eq_pos = ft_strchr(shell->envp[i], '=');
		if (eq_pos && (eq_pos - shell->envp[i]) == len)
			if (ft_strncmp(shell->envp[i], var_name, len) == 0)
				return (eq_pos + 1);
		i++;
	}
	return (NULL);
}

