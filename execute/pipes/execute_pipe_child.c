/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe_child.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 22:25:13 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:30:44 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	setup_pipe_redirection(int cmd_index, int cmd_count, int **pipes)
{
	if (cmd_index > 0)
		dup2(pipes[cmd_index - 1][0], STDIN_FILENO);
	if (cmd_index < cmd_count - 1)
		dup2(pipes[cmd_index][1], STDOUT_FILENO);
}

static void	cleanup_pipe_descriptors(int **pipes, int cmd_count)
{
	int	i;

	i = 0;
	while (i < cmd_count - 1)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

static char	**process_argument_expansion(char **args, t_shell *shell)
{
	char	*reconstructed;

	args = expand_args_variables(args, shell);
	reconstructed = reconstruct_args_string(args);
	if (!reconstructed)
	{
		process_args_quotes(args, shell);
		return (args);
	}
	reconstructed = apply_wildcard_expansion(reconstructed);
	args = execute_expanded_args_split(reconstructed, args, shell);
	free(reconstructed);
	return (args);
}

void	execute_pipe_child(t_split cmd, t_pipe_child_context *ctx,
		t_shell *shell)
{
	char	**args;

	setup_pipe_redirection(ctx->cmd_index, ctx->cmd_count, ctx->pipes);
	cleanup_pipe_descriptors(ctx->pipes, ctx->cmd_count);
	setup_child_signals();
	if (has_parentheses_in_split(cmd))
	{
		parser_and_or(shell, cmd);
		exit(shell->past_exit_status);
	}
	args = split_to_args(cmd);
	if (!args || !args[0])
		exit(1);
	args = process_argument_expansion(args, shell);
	if (!args || !args[0])
		exit(0);
	if (is_builtin(args[0]))
		exit(execute_builtin(args, shell));
	else
		execute_pipe_external_command(args, shell);
}
