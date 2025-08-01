/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 14:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 17:04:46 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	append_double_quotes(char *string)
{
	char	*pointer_for_traversal;
	int		flag_for_equal_sign;

	flag_for_equal_sign = 0;
	write(1, "declare -x ", 11);
	pointer_for_traversal = string;
	while (*pointer_for_traversal)
	{
		write(1, pointer_for_traversal, 1);
		if (*pointer_for_traversal == '=')
		{
			write(1, "\"", 1);
			flag_for_equal_sign = 1;
		}
		pointer_for_traversal++;
	}
	if (flag_for_equal_sign)
		write(1, "\"\n", 2);
	else
		write(1, "\n", 1);
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
	return (new_env_var(var_name, value, shell));
}
