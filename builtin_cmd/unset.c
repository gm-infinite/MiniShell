/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:35:56 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 16:15:03 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	unset_check_and_remove(t_shell *shell, char *var_name, int len,
							int index)
{
	char	*eq_pos;

	eq_pos = ft_strchr(shell->envp[index], '=');
	if (eq_pos && (eq_pos - shell->envp[index]) == len)
	{
		if (ft_strncmp(shell->envp[index], var_name, len) == 0)
			return (1);
	}
	return (0);
}

static int	copy_env_without_var(t_shell *shell, int remove_index, int count)
{
	char	**new_envp;
	int		j;
	int		k;

	new_envp = malloc(sizeof(char *) * count);
	if (!new_envp)
		return (-1);
	j = 0;
	k = 0;
	while (k < count)
	{
		if (k == remove_index)
		{
			free(shell->envp[k]);
			k++;
			continue;
		}
		new_envp[j] = shell->envp[k];
		j++;
		k++;
	}
	new_envp[j] = NULL;
	free(shell->envp);
	shell->envp = new_envp;
	return (0);
}

int	unset_env_var(char *var_name, t_shell *shell)
{
	int		i;
	int		j;
	int		len;
	int		count;

	if (!var_name || !shell->envp)
		return (1);
	len = ft_strlen(var_name);
	count = 0;
	while (shell->envp[count])
		count++;
	i = 0;
	while (shell->envp[i])
	{
		if (unset_check_and_remove(shell, var_name, len, i))
		{
			j = copy_env_without_var(shell, i, count);
			if (j == -1)
				return (1);
			return (0);
		}
		i++;
	}
	return (1);
}

int	builtin_unset(char **args, t_shell *shell)
{
	int	i;

	if (!args[1])
		return (0);
	i = 1;
	while (args[i])
	{
		if (args[i][0] == '-')
		{
			write(STDERR_FILENO, "bash: unset: ", 13);
			write(STDERR_FILENO, args[i], ft_strlen(args[i]));
			write(STDERR_FILENO, ": invalid option\n", 17);
			write(STDERR_FILENO, "unset: usage: unset [-f] [-v] [-n] [name ...]\n", 47);
			shell->past_exit_status = 2;
			return (2);
		}
		if (is_valid_var_name(args[i]))
			unset_env_var(args[i], shell);
		i++;
	}
	return (0);
}
