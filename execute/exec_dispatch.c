/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_dispatch.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 16:22:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:23:24 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	execute_args_array(t_split split, t_shell *shell)
{
	char	**args;
	char	*reconstructed;
	char	*wildcard_expanded;
	t_split	new_split;
	int		exit_status;

	args = split_to_args(split);
	if (!args)
		return (1);

	// First pass: expand variables in each argument
	int i = 0;
	while (args[i])
	{
		char *expanded = expand_variables_quoted(args[i], shell);
		if (expanded != args[i])
		{
			free(args[i]);
			args[i] = expanded;
		}
		i++;
	}

	// Reconstruct the command string for wildcard expansion
	reconstructed = NULL;
	i = 0;
	while (args[i])
	{
		if (reconstructed == NULL)
			reconstructed = ft_strdup(args[i]);
		else
		{
			char *temp = ft_strjoin(reconstructed, " ");
			free(reconstructed);
			reconstructed = temp;
			temp = ft_strjoin(reconstructed, args[i]);
			free(reconstructed);
			reconstructed = temp;
		}
		i++;
	}

	free_args(args);

	if (!reconstructed)
		return (1);

	// Apply wildcard expansion
	wildcard_expanded = wildcard_input_modify(reconstructed);
	if (wildcard_expanded && wildcard_expanded != reconstructed)
	{
		free(reconstructed);
		reconstructed = wildcard_expanded;
	}

	// Split again with wildcard expansion applied
	new_split = create_split_str(reconstructed);
	free(reconstructed);

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

static int	dispatch_execution(t_split split, t_shell *shell,
							int has_pipes, int has_redir)
{
	if (has_pipes > 0 && has_redir)
		return (execute_pipeline_with_redirections(split, shell));
	else if (has_pipes > 0)
		return (execute_pipeline(split, shell));
	else if (has_redir)
		return (execute_with_redirections(split, shell));
	else
		return (execute_args_array(split, shell));
}

int	execute_command(t_split split, t_shell *shell)
{
	int	exit_status;
	int	has_pipes;
	int	has_redir;
	int	syntax_error;

	if (split.size == 0)
		return (0);
	syntax_error = validate_redirection_syntax(split);
	if (syntax_error != 0)
	{
		shell->past_exit_status = syntax_error;
		return (syntax_error);
	}
	has_pipes = count_pipes(split);
	has_redir = has_redirections(split);
	exit_status = dispatch_execution(split, shell, has_pipes, has_redir);
	shell->past_exit_status = exit_status;
	return (exit_status);
}
