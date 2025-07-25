/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 14:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 18:41:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

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
	return (new_env_var(var_name, value, shell));
}
