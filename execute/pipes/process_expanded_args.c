/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_expanded_args.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 15:51:01 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 17:09:46 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

char	**execute_expanded_args_split(char *reconstructed, char **args,
		t_shell *shell)
{
	char	*wildcard_expanded;
	char	*to_process;
	t_split	new_split;

	wildcard_expanded = wildcard_input_modify(reconstructed);
	if (wildcard_expanded && wildcard_expanded != reconstructed)
		to_process = wildcard_expanded;
	else
		to_process = reconstructed;
	new_split = create_split_str(to_process);
	if (wildcard_expanded && wildcard_expanded != reconstructed)
		free(wildcard_expanded);
	if (new_split.size == 0)
	{
		free_split(&new_split);
		process_args_quotes(args, shell);
		return (args);
	}
	free_args(args);
	args = split_to_args(new_split);
	free_split(&new_split);
	if (args)
		process_args_quotes(args, shell);
	return (args);
}

char	**process_argument_expansion(char **args, t_shell *shell)
{
	char	*reconstructed;

	args = arg_expander_loop(args, shell);
	reconstructed = reconstructed_args(args);
	if (!reconstructed)
	{
		process_args_quotes(args, shell);
		return (args);
	}
	reconstructed = wildcard_expand(reconstructed);
	args = execute_expanded_args_split(reconstructed, args, shell);
	free(reconstructed);
	return (args);
}
