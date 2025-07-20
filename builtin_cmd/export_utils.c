/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 14:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 16:15:03 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	create_new_env_var(char *var_name, char *value, t_shell *shell)
{
	char	**new_envp;
	char	*temp;
	int		i;

	i = 0;
	while (shell->envp[i])
		i++;
	new_envp = malloc(sizeof(char *) * (i + 2));
	if (!new_envp)
		return (1);
	ft_memcpy(new_envp, shell->envp, sizeof(char *) * i);
	temp = ft_strjoin(var_name, "=");
	if (!temp)
	{
		free(new_envp);
		return (1);
	}
	new_envp[i] = ft_strjoin(temp, value);
	free(temp);
	if (!new_envp[i])
	{
		free(new_envp);
		return (1);
	}
	new_envp[i + 1] = NULL;
	free(shell->envp);
	shell->envp = new_envp;
	return (0);
}

static int	update_existing_var(int index, char *var_name, char *value,
						t_shell *shell)
{
	char	*temp;

	temp = ft_strjoin(var_name, "=");
	if (!temp)
		return (1);
	free(shell->envp[index]);
	shell->envp[index] = ft_strjoin(temp, value);
	free(temp);
	if (!shell->envp[index])
		return (1);
	return (0);
}

int	set_env_var(char *var_name, char *value, t_shell *shell)
{
	int		i;
	int		len;
	char	*eq_pos;

	if (!var_name || !value)
		return (1);
	len = ft_strlen(var_name);
	i = 0;
	while (shell->envp[i])
	{
		eq_pos = ft_strchr(shell->envp[i], '=');
		if (eq_pos && (eq_pos - shell->envp[i]) == len)
		{
			if (ft_strncmp(shell->envp[i], var_name, len) == 0)
				return (update_existing_var(i, var_name, value, shell));
		}
		i++;
	}
	return (create_new_env_var(var_name, value, shell));
}

static int	append_existing_var(int index, char *var_name, char *value,
						t_shell *shell)
{
	char	*current_value;
	char	*new_value;
	char	*temp;

	current_value = ft_strdup(ft_strchr(shell->envp[index], '=') + 1);
	if (!current_value)
		return (1);
	new_value = ft_strjoin(current_value, value);
	free(current_value);
	if (!new_value)
		return (1);
	temp = ft_strjoin(var_name, "=");
	if (!temp)
	{
		free(new_value);
		return (1);
	}
	free(shell->envp[index]);
	shell->envp[index] = ft_strjoin(temp, new_value);
	free(temp);
	free(new_value);
	if (!shell->envp[index])
		return (1);
	return (0);
}

int	append_env_var(char *var_name, char *value, t_shell *shell)
{
	int		i;
	int		len;
	char	*eq_pos;

	if (!var_name || !value)
		return (1);
	len = ft_strlen(var_name);
	i = 0;
	while (shell->envp[i])
	{
		eq_pos = ft_strchr(shell->envp[i], '=');
		if (eq_pos && (eq_pos - shell->envp[i]) == len)
		{
			if (ft_strncmp(shell->envp[i], var_name, len) == 0)
				return (append_existing_var(i, var_name, value, shell));
		}
		i++;
	}
	return (set_env_var(var_name, value, shell));
}
