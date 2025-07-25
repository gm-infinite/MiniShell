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

static int	dispatch_execution(t_split split, t_shell *shell,
		int has_pipes, int has_redir)
{
	if (has_pipes > 0 && has_redir)
		return (execute_pipe_redir(split, shell));
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
	has_redir = has_redirs(split);
	exit_status = dispatch_execution(split, shell, has_pipes, has_redir);
	shell->past_exit_status = exit_status;
	return (exit_status);
}
