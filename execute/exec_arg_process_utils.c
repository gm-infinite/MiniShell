/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_arg_process_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 16:22:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:23:24 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

char	*expandvar_quoted(char *str, t_shell *shell)
{
	char	*result;
	char	*tilde_expanded;

	if (!str)
		return (NULL);
	tilde_expanded = tilde(str, shell);
	if (!tilde_expanded)
		return (NULL);
	result = expandvar_q(tilde_expanded, shell);
	if (result != tilde_expanded && tilde_expanded)
		free(tilde_expanded);
	return (result);
}

char	**arg_expander_loop(char **args, t_shell *shell)
{
	int		i;
	char	*expanded;

	i = -1;
	while (args[++i])
	{
		expanded = expandvar_quoted(args[i], shell);
		if (expanded != args[i])
		{
			free(args[i]);
			args[i] = expanded;
		}
	}
	return (args);
}

char	*reconstructed_args(char **args)
{
	char	*reconstructed;
	char	*temp;
	int		i;

	reconstructed = NULL;
	i = -1;
	while (args[++i])
	{
		if (reconstructed == NULL)
			reconstructed = ft_strdup(args[i]);
		else
		{
			temp = ft_strjoin(reconstructed, " ");
			free(reconstructed);
			reconstructed = temp;
			temp = ft_strjoin(reconstructed, args[i]);
			free(reconstructed);
			reconstructed = temp;
		}
	}
	return (reconstructed);
}

char	*wildcard_expand(char *reconstructed)
{
	char	*wildcard_expanded;

	wildcard_expanded = wildcard_input_modify(reconstructed);
	if (wildcard_expanded && wildcard_expanded != reconstructed)
	{
		free(reconstructed);
		return (wildcard_expanded);
	}
	return (reconstructed);
}

int	execute_expanded_args(char *reconstructed, t_shell *shell)
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
