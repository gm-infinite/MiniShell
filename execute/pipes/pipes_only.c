/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_only.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:10:57 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 10:42:37 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static t_pipe_cleaner	cleanup_assignment(t_child_params *params)
{
	t_pipe_cleaner	ret;

	ret.commands = params->commands;
	ret.pipes = params->ctx->pipes;
	ret.pids = params->pids;
	ret.cmd_count = params->ctx->cmd_count;
	return (ret);
}

static int	handle_single_command(t_split *commands, t_shell *shell)
{
	char	**args;
	int		exit_status;

	args = split_to_args(commands[0]);
	if (!args)
	{
		free(commands);
		return (1);
	}
	exit_status = execute_single_command(args, shell);
	free_args(args);
	free(commands);
	return (exit_status);
}

static void	execute_pipe_child_process(t_child_params *params)
{
	char			**args;
	int				builtin_result;
	t_pipe_cleaner	cleanup;

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

void	execute_pipe_child(t_child_params *params)
{
	int	in_fd;
	int	out_fd;
	int	j;

	in_fd = -1;
	out_fd = -1;
	if (params->ctx->cmd_index > 0)
	{
		in_fd = params->ctx->pipes[params->ctx->cmd_index - 1][0];
		dup2(in_fd, STDIN_FILENO);
	}
	if (params->ctx->cmd_index < params->ctx->cmd_count - 1)
	{
		out_fd = params->ctx->pipes[params->ctx->cmd_index][1];
		dup2(out_fd, STDOUT_FILENO);
	}
	j = -1;
	while (++j < params->ctx->cmd_count - 1)
	{
		close(params->ctx->pipes[j][0]);
		close(params->ctx->pipes[j][1]);
	}
	setup_child_signals();
	execute_pipe_child_process(params);
	exit(127);
}

int	execute_pipeline(t_split split, t_shell *shell)
{
	t_split	*commands;
	int		cmd_count;
	int		result;

	commands = split_by_pipes(split, &cmd_count);
	if (!commands)
		return (1);
	if (cmd_count == 1)
		result = handle_single_command(commands, shell);
	else
		result = execute_multi_command_pipeline(commands, cmd_count, shell);
	return (result);
}
