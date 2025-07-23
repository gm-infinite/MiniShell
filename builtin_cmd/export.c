/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:36:19 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/23 12:33:09 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static void	sort_env_array(char **sorted_env, int count)
{
	int		i;
	int		j;
	char	*temp;

	i = -1;
	while (++i < count - 1)
	{
		j = -1;
		while (++j < count - i - 1)
		{
			if (ft_strncmp(sorted_env[j], sorted_env[j + 1],
					ft_strlen(sorted_env[j])) > 0)
			{
				temp = sorted_env[j];
				sorted_env[j] = sorted_env[j + 1];
				sorted_env[j + 1] = temp;
			}
		}
	}
}

static int	print_sorted_env(t_shell *shell)
{
	char	**sorted_env;
	int		count;
	int		i;
	int		printer;

	count = 0;
	while (shell->envp[count])
		count++;
	sorted_env = malloc(sizeof(char *) * count);
	if (!sorted_env)
		return (1);
	i = -1;
	while (++i < count)
		sorted_env[i] = shell->envp[i];
	sort_env_array(sorted_env, count);
	printer = 0;
	while (printer < count)
	{
		if (!(sorted_env[printer][0] == '_' && sorted_env[printer][1] == '='))
			printf("declare -x %s\n", sorted_env[printer]);
		printer++;
	}
	free(sorted_env);
	return (0);
}

static int	process_export_args(char **args, t_shell *shell)
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
			print_option_error(args[i], shell);
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

int	builtin_export(char **args, t_shell *shell)
{
	if (!args[1])
		return (print_sorted_env(shell));
	return (process_export_args(args, shell));
}
