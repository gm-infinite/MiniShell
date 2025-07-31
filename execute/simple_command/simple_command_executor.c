/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simple_command_executor.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 11:14:15 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 14:18:14 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static int	execute_expanded_args(char *reconstructed, t_shell *shell)
{
	t_split	new_split;
	char	**args;
	int		exit_status;

	new_split = create_split_str(reconstructed);
	if (new_split.size == 0)
	{
		free_split(&new_split);
		return (1);
	}
	args = split_to_args(new_split);
	free_split(&new_split);
	if (!args)
		return (1);
	exit_status = execute_single_command(args, shell);
	free_args(args);
	return (exit_status);
}

int	execute_args_array(t_split split, t_shell *shell)
{
	char	**args;
	char	*reconstructed;
	int		exit_status;

	args = split_to_args(split);
	if (!args)
		return (1);
	args = arg_expander_loop(args, shell);
	reconstructed = reconstructed_args(args);
	free_args(args);
	if (!reconstructed)
		return (1);
	reconstructed = wildcard_expand(reconstructed);
	exit_status = execute_expanded_args(reconstructed, shell);
	free(reconstructed);
	return (exit_status);
}
