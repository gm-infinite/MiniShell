/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_process.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 14:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 16:15:03 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	process_assign_export(char *arg, char *eq_pos, t_shell *shell)
{
	char	*var_name;
	char	*var_value;
	int		result;

	var_name = ft_substr(arg, 0, eq_pos - arg);
	if (!var_name)
		return (1);
	if (!is_valid_var_name(var_name))
	{
		write(STDERR_FILENO, "export: `", 9);
		write(STDERR_FILENO, arg, ft_strlen(arg));
		write(STDERR_FILENO, "': not a valid identifier\n", 26);
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

static int	process_append_export(char *arg, char *eq_pos, t_shell *shell)
{
	char	*var_name;
	char	*var_value;
	int		result;

	var_name = ft_substr(arg, 0, eq_pos - arg - 1);
	if (!var_name)
		return (1);
	if (!is_valid_var_name(var_name))
	{
		write(STDERR_FILENO, "export: `", 9);
		write(STDERR_FILENO, arg, ft_strlen(arg));
		write(STDERR_FILENO, "': not a valid identifier\n", 26);
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

static int	process_single_export(char *arg, t_shell *shell)
{
	if (!is_valid_var_name(arg))
	{
		write(STDERR_FILENO, "export: `", 9);
		write(STDERR_FILENO, arg, ft_strlen(arg));
		write(STDERR_FILENO, "': not a valid identifier\n", 26);
		return (1);
	}
	return (set_env_var(arg, "", shell));
}

int	process_export_args(char **args, t_shell *shell)
{
	int		i;
	int		exit_status;
	char	*eq_pos;

	i = 1;
	exit_status = 0;
	while (args[i])
	{
		if (args[i][0] == '-')
		{
			write(STDERR_FILENO, "bash: export: ", 14);
			write(STDERR_FILENO, args[i], ft_strlen(args[i]));
			write(STDERR_FILENO, ": invalid option\n", 17);
			shell->past_exit_status = 2;
			return (2);
		}
		eq_pos = ft_strchr(args[i], '=');
		if (eq_pos && eq_pos > args[i] && *(eq_pos - 1) == '+')
			exit_status |= process_append_export(args[i], eq_pos, shell);
		else if (eq_pos)
			exit_status |= process_assign_export(args[i], eq_pos, shell);
		else
			exit_status |= process_single_export(args[i], shell);
		i++;
	}
	shell->past_exit_status = exit_status;
	return (exit_status);
}
