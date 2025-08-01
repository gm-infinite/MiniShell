/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_append.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 14:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/25 11:17:19 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static int	append_existing_var(int index, char *var_name, char *value,
							t_shell *shell)
{
	char	*eq_pos;
	char	*current_value;
	char	*new_value;
	char	*temp;
	char	*result;

	eq_pos = ft_strchr(shell->envp[index], '=');
	current_value = ft_strdup(eq_pos + 1);
	if (!current_value)
		return (1);
	new_value = ft_strjoin(current_value, value);
	free(current_value);
	if (!new_value)
		return (1);
	free(shell->envp[index]);
	temp = ft_strjoin(var_name, "=");
	if (!temp)
		return (1);
	result = ft_strjoin(temp, new_value);
	free(temp);
	shell->envp[index] = result;
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
	i = -1;
	while (shell->envp[++i])
	{
		eq_pos = ft_strchr(shell->envp[i], '=');
		if (eq_pos && (eq_pos - shell->envp[i]) == len)
			if (ft_strncmp(shell->envp[i], var_name, len) == 0)
				return (append_existing_var(i, var_name, value, shell));
	}
	return (set_env_var(var_name, value, shell));
}
