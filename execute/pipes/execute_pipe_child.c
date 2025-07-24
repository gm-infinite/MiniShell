/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe_child.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 22:25:13 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 14:38:58 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

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

static t_pipeline_cleanup	cleanup_assignment(t_pipe_child_params *params)
{
	t_pipeline_cleanup	ret;

	ret.commands = params->commands;
	ret.pipes = params->ctx->pipes;
	ret.pids = params->pids;
	ret.cmd_count = params->ctx->cmd_count;
	return (ret);
}

void	execute_pipe_child_process(t_pipe_child_params *params)
{
	char				**args;
	int					builtin_result;
	t_pipeline_cleanup	cleanup;

	args = split_to_args(params->cmd);
	cleanup = cleanup_assignment(params);
	if (!args || !args[0])
	{
		free_child_pipeline_memory(args, params->shell, &cleanup);
		exit(1);
	}
	args = process_argument_expansion(args, params->shell);
	if (!args || !args[0])
	{
		free_child_pipeline_memory(args, params->shell, &cleanup);
		exit(0);
	}
	if (is_builtin(args[0]))
	{
		builtin_result = execute_builtin(args, params->shell);
		free_child_pipeline_memory(args, params->shell, &cleanup);
		exit(builtin_result);
	}
	else
		execute_pipe_external_command(args, params->shell, &cleanup);
}

void	execute_pipe_child(t_pipe_child_params *params)
{
	t_pipeline_cleanup	cleanup;

	cleanup = cleanup_assignment(params);
	setup_pipe_redirection(params->ctx->cmd_index, params->ctx->cmd_count,
		params->ctx->pipes);
	cleanup_pipe_descriptors(params->ctx->pipes, params->ctx->cmd_count);
	setup_child_signals();
	if (has_parentheses_in_split(params->cmd))
	{
		cleanup = cleanup_assignment(params);
		parser_and_or(params->shell, params->cmd);
		free_child_pipeline_memory(NULL, params->shell, &cleanup);
		exit(params->shell->past_exit_status);
	}
	execute_pipe_child_process(params);
}
