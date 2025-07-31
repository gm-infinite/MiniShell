/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:35:56 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/25 11:19:34 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static int	unset_env_var(char *var_name, t_shell *shell)
{
	int	index;
	int	count;

	if (!var_name || !shell->envp)
		return (1);
	count = count_env_vars(shell);
	index = find_env_var_index(shell, var_name, ft_strlen(var_name));
	if (index == -1)
		return (1);
	if (copy_env_without_var(shell, index, count) == -1)
		return (1);
	return (0);
}

static void	write_error_unset_option(char *arg)
{
	write(STDERR_FILENO, "bash: unset: ", 13);
	write(STDERR_FILENO, arg, ft_strlen(arg));
	write(STDERR_FILENO, ": invalid option\n", 17);
	write(STDERR_FILENO, "unset: usage: unset [-f] [-v] [-n] [name ...]\n", 47);
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
			write_error_unset_option(args[i]);
			return (2);
		}
		if (is_valid_var_name(args[i]))
			unset_env_var(args[i], shell);
		i++;
	}
	return (0);
}
