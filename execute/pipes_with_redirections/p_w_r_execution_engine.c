/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   p_w_r_execution_engine.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 16:41:24 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 17:21:11 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	execute_pipeline_external_command(char **args, char *executable,
		t_shell *shell, t_pipe_cleaner *cleanup)
{
	execve(executable, args, shell->envp);
	perror("execve");
	free_child_pipeline_memory(args, shell, cleanup);
	exit(127);
}

static void	fd_initializer(t_child_redir_params *params, int *fd_values)
{
	fd_values[0] = STDIN_FILENO;
	fd_values[1] = STDOUT_FILENO;
	fd_values[2] = STDERR_FILENO;
	fd_values[3] = params->ctx->heredoc_fds[params->ctx->cmd_index];
	if (fd_values[3] >= 0)
		fd_values[0] = fd_values[3];
}

static void	error_pipe_exec(char **args, char *executable,
		t_shell *shell, t_pipe_cleaner *cleanup)
{
	if (!executable)
	{
		write(STDERR_FILENO, args[0], ft_strlen(args[0]));
		write(STDERR_FILENO, ": command not found\n", 20);
		free_child_pipeline_memory(args, shell, cleanup);
		exit(127);
	}
	if (ft_strncmp(executable, "__NOT_DIRECTORY__", 17) == 0)
	{
		write_notdir(args[0]);
		free(executable);
		free_child_pipeline_memory(args, shell, cleanup);
		exit(126);
	}
}

static void	execute_child_command_with_pipeline_cleanup(char **args,
			t_shell *shell, t_pipe_cleaner *cleanup)
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
	error_pipe_exec(args, executable, shell, cleanup);
	execute_pipeline_external_command(args, executable, shell, cleanup);
}

void	execute_child_redir(t_child_redir_params *params)
{
	char				**args;
	t_redir_fds			fds;
	t_pipe_setup_ctx	p;
	int					fd_values[4];
	t_pipe_cleaner		cleanup;

	fd_initializer(params, fd_values);
	fds = get_fds(fd_values);
	fds.preprocessed_heredoc = (fd_values[3] >= 0);
	p = get_pipe_ctx(params->ctx->cmd_index, params->ctx->cmd_count,
			params->ctx->pipes, fd_values);
	setup_pipe_fds(&p);
	args = parse_redirs(params->cmd, &fds, params->shell);
	cleanup = get_cleanup(params->commands, p.pipes, params->pids, p.cmd_count);
	if (!args)
	{
		free_child_pipeline_memory(NULL, params->shell, &cleanup);
		exit(1);
	}
	redirect_fds(fd_values[0], fd_values[1], fd_values[2]);
	close_all_pipes(params->ctx->pipes, params->ctx->cmd_count);
	setup_child_signals();
	process_and_check_args(args, params->shell);
	execute_child_command_with_pipeline_cleanup(args, params->shell, &cleanup);
}

int	fork_pipe_child(t_pipe_ctx *pipeline_ctx, int i)
{
	t_child_ctx				ctx;
	t_child_redir_params	params;

	pipeline_ctx->pids[i] = fork();
	if (pipeline_ctx->pids[i] == -1)
	{
		perror("fork");
		return (1);
	}
	else if (pipeline_ctx->pids[i] == 0)
	{
		ctx.pipes = pipeline_ctx->pipes;
		ctx.cmd_index = i;
		ctx.cmd_count = pipeline_ctx->cmd_count;
		ctx.heredoc_fds = pipeline_ctx->heredoc_fds;
		params.cmd = pipeline_ctx->commands[i];
		params.ctx = &ctx;
		params.shell = pipeline_ctx->shell;
		params.commands = pipeline_ctx->commands;
		params.pids = pipeline_ctx->pids;
		execute_child_redir(&params);
		exit(127);
	}
	return (0);
}
