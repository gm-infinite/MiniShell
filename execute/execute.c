/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:50:25 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/23 19:32:33 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	execute_pipeline_with_redirections(t_split split, t_shell *shell)
{
	t_split	*commands;
	int		cmd_count;
	int		exit_status;

	commands = split_by_pipes(split, &cmd_count);
	if (!commands)
		return (1);
	if (cmd_count == 1)
	{
		exit_status = execute_with_redirections(commands[0], shell);
		free(commands);
		return (exit_status);
	}
	return (setup_and_execute_pipeline(commands, cmd_count, shell));
}

static void	execute_child_command_with_pipeline_cleanup(char **args, t_shell *shell, 
			t_pipeline_cleanup *cleanup)
{
	char	*executable;
	int		builtin_result;

	if (is_builtin(args[0]))
	{
		builtin_result = execute_builtin(args, shell);
		free_child_pipeline_memory(args, shell, cleanup);
		exit(builtin_result);
	}
	executable = find_executable(args[0], shell);
	if (!executable)
	{
		write_exec_error_message(args[0], ": command not found\n");
		free_child_pipeline_memory(args, shell, cleanup);
		exit(127);
	}
	execve(executable, args, shell->envp);
	perror("execve");
	free_child_pipeline_memory(args, shell, cleanup);
	exit(127);
}

void	execute_pipe_child_with_redirections(t_pipe_child_redir_params *params)
{
	char					**args;
	t_redir_fds				fds;
	t_pipe_setup_context	pipe_ctx;
	int						fd_values[3];
	t_pipeline_cleanup		cleanup;

	fd_values[0] = STDIN_FILENO;
	fd_values[1] = STDOUT_FILENO;
	fd_values[2] = STDERR_FILENO;
	fds.input_fd = &fd_values[0];
	fds.output_fd = &fd_values[1];
	fds.stderr_fd = &fd_values[2];
	pipe_ctx.cmd_index = params->ctx->cmd_index;
	pipe_ctx.cmd_count = params->ctx->cmd_count;
	pipe_ctx.pipes = params->ctx->pipes;
	pipe_ctx.input_fd = &fd_values[0];
	pipe_ctx.output_fd = &fd_values[1];
	setup_pipe_fds(&pipe_ctx);
	args = parse_redirections(params->cmd, &fds, params->shell);
	if (!args)
	{
		cleanup.commands = params->commands;
		cleanup.pipes = params->ctx->pipes;
		cleanup.pids = params->pids;
		cleanup.cmd_count = params->ctx->cmd_count;
		free_child_pipeline_memory(NULL, params->shell, &cleanup);
		exit(1);
	}
	redirect_fds(fd_values[0], fd_values[1], fd_values[2]);
	close_all_pipes(params->ctx->pipes, params->ctx->cmd_count);
	setup_child_signals();
	process_and_check_args(args, params->shell);
	cleanup.commands = params->commands;
	cleanup.pipes = params->ctx->pipes;
	cleanup.pids = params->pids;
	cleanup.cmd_count = params->ctx->cmd_count;
	execute_child_command_with_pipeline_cleanup(args, params->shell, &cleanup);
}
