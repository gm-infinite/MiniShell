/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_process_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 22:30:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 13:32:22 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

void	option_error(char *arg, t_shell *shell)
{
	write(STDERR_FILENO, "bash: export: ", 14);
	write(STDERR_FILENO, arg, ft_strlen(arg));
	write(STDERR_FILENO, ": invalid option\n", 17);
	shell->past_exit_status = 2;
}

int	process_assign_export(char *arg, char *eq_pos, t_shell *shell)
{
	char	*var_name;
	char	*var_value;
	int		result;

	var_name = ft_substr(arg, 0, eq_pos - arg);
	if (!var_name)
		return (1);
	if (!is_valid_var_name(var_name))
	{
		export_error(arg);
		free(var_name);
		return (1);
	}
	var_value = ft_strdup(eq_pos + 1);
	if (!var_value)
	{
		free(var_name);
		return (1);
	}
	result = set_env_var(var_name, var_value, shell);
	free(var_name);
	free(var_value);
	return (result);
}

int	process_append_export(char *arg, char *eq_pos, t_shell *shell)
{
	char	*var_name;
	char	*var_value;
	int		result;

	var_name = ft_substr(arg, 0, eq_pos - arg - 1);
	if (!var_name)
		return (1);
	if (!is_valid_var_name(var_name))
	{
		export_error(arg);
		free(var_name);
		return (1);
	}
	var_value = ft_strdup(eq_pos + 1);
	if (!var_value)
	{
		free(var_name);
		return (1);
	}
	result = append_env_var(var_name, var_value, shell);
	free(var_name);
	free(var_value);
	return (result);
}

static int	check_existing_var(char *arg, int len, t_shell *shell)
{
	int		i;
	char	*eq_pos;

	i = 0;
	while (shell->envp[i])
	{
		eq_pos = ft_strchr(shell->envp[i], '=');
		if (eq_pos && (eq_pos - shell->envp[i]) == len)
		{
			if (ft_strncmp(shell->envp[i], arg, len) == 0)
				return (1);
		}
		else if (!eq_pos && (int)ft_strlen(shell->envp[i]) == len)
		{
			if (ft_strncmp(shell->envp[i], arg, len) == 0)
				return (1);
		}
		i++;
	}
	return (0);
}

int	process_single_export(char *arg, t_shell *shell)
{
	int	len;

	if (!is_valid_var_name(arg))
	{
		export_error(arg);
		return (1);
	}
	len = ft_strlen(arg);
	if (check_existing_var(arg, len, shell))
		return (0);
	return (new_empty_envvar(arg, shell));
}
