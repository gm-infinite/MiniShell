/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:35:56 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:27:27 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	unset_env_var(char *var_name, t_shell *shell)
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
			fprintf(stderr, "bash: unset: %s: invalid option\n", args[i]);
			fprintf(stderr, "unset: usage: unset [-f] [-v] [-n] [name ...]\n");
			shell->past_exit_status = 2;
			return (2);
		}
		if (is_valid_var_name(args[i]))
			unset_env_var(args[i], shell);
		i++;
	}
	return (0);
}
