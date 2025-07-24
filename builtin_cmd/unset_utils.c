/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 00:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/23 10:51:48 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	unset_check_and_remove(t_shell *shell, char *var_name, int len, int index)
{
	char	*eq_pos;

	eq_pos = ft_strchr(shell->envp[index], '=');
	if (eq_pos && (eq_pos - shell->envp[index]) == len)
	{
		if (ft_strncmp(shell->envp[index], var_name, len) == 0)
			return (1);
	}
	else if (!eq_pos && (int)ft_strlen(shell->envp[index]) == len)
	{
		if (ft_strncmp(shell->envp[index], var_name, len) == 0)
			return (1);
	}
	return (0);
}

static void	copy_env_entries(t_shell *shell, char **new_envp, int remove_index,
	int count)
{
	int	j;
	int	k;

	j = 0;
	k = 0;
	while (k < count)
	{
		if (k == remove_index)
		{
			free(shell->envp[k]);
			k++;
			continue ;
		}
		new_envp[j] = shell->envp[k];
		j++;
		k++;
	}
	new_envp[j] = NULL;
}

int	copy_env_without_var(t_shell *shell, int remove_index, int count)
{
	char	**new_envp;

	new_envp = malloc(sizeof(char *) * count);
	if (!new_envp)
		return (-1);
	copy_env_entries(shell, new_envp, remove_index, count);
	free(shell->envp);
	shell->envp = new_envp;
	return (0);
}

int	find_env_var_index(t_shell *shell, char *var_name, int len)
{
	int	i;

	i = 0;
	while (shell->envp[i])
	{
		if (unset_check_and_remove(shell, var_name, len, i))
			return (i);
		i++;
	}
	return (-1);
}

int	count_env_vars(t_shell *shell)
{
	int	count;

	count = 0;
	while (shell->envp[count])
		count++;
	return (count);
}
