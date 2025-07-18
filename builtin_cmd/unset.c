/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:35:56 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/16 20:58:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	unset_env_var(char *var_name, t_shell *shell)
{
	int		i;
	int		j;
	int		len;
	int		count;
	char	*eq_pos;
	char	**new_envp;

	if (!var_name || !shell->envp)
		return (1);
	
	len = ft_strlen(var_name);
	count = 0;
	while (shell->envp[count])
		count++;
	
	// Find and remove the variable
	i = 0;
	while (shell->envp[i])
	{
		eq_pos = ft_strchr(shell->envp[i], '=');
		if (eq_pos && (eq_pos - shell->envp[i]) == len)
		{
			if (ft_strncmp(shell->envp[i], var_name, len) == 0)
			{
				// Found variable to remove
				new_envp = malloc(sizeof(char *) * count);
				if (!new_envp)
					return (1);
				
				// Copy all except the one to remove
				j = 0;
				while (j < i)
				{
					new_envp[j] = shell->envp[j];
					j++;
				}
				free(shell->envp[i]); // Free the removed variable
				while (j < count - 1)
				{
					new_envp[j] = shell->envp[j + 1];
					j++;
				}
				new_envp[j] = NULL;
				
				free(shell->envp);
				shell->envp = new_envp;
				return (0);
			}
		}
		i++;
	}
	return (1); // Variable not found
}

int	builtin_unset(char **args, t_shell *shell)
{
	int	i;

	if (!args[1])
		return (0);
	
	i = 1;
	while (args[i])
	{
		unset_env_var(args[i], shell);
		i++;
	}
	
	return (0);
}
