/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:50:25 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 14:14:02 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static void	execute_child_command_with_pipeline_cleanup(char **args,
			t_shell *shell, t_pipeline_cleanup *cleanup)
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

static t_pipe_setup_context	get_pipe_ctx(int cmd_index, int cmd_count,
	int **pipes, int *fd_values)
{
	t_pipe_setup_context	ret;

	ret.cmd_count = cmd_count;
	ret.cmd_index = cmd_index;
	ret.pipes = pipes;
	ret.input_fd = &(fd_values[0]);
	ret.output_fd = &(fd_values[1]);
	return (ret);
}

static t_pipeline_cleanup	get_cleanup(t_split *commands, int **pipes,
		pid_t *pids, int cmd_count)
{
	t_pipeline_cleanup	ret;

	ret.commands = commands;
	ret.pipes = pipes;
	ret.pids = pids;
	ret.cmd_count = cmd_count;
	return (ret);
}

static t_redir_fds	get_fds(int *fd_values)
{
	t_redir_fds	ret;

	ret.input_fd = &fd_values[0];
	ret.output_fd = &fd_values[1];
	ret.stderr_fd = &fd_values[2];
	return (ret);
}

void	execute_pipe_child_with_redirections(t_pipe_child_redir_params *params)
{
	char					**args;
	t_redir_fds				fds;
	t_pipe_setup_context	p;
	int						fd_values[3];
	t_pipeline_cleanup		cleanup;

	fd_values[0] = STDIN_FILENO;
	fd_values[1] = STDOUT_FILENO;
	fd_values[2] = STDERR_FILENO;
	fds = get_fds(fd_values);
	p = get_pipe_ctx(params->ctx->cmd_index, params->ctx->cmd_count,
			params->ctx->pipes, fd_values);
	setup_pipe_fds(&p);
	args = parse_redirections(params->cmd, &fds, params->shell);
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
