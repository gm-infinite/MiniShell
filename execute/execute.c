/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:50:25 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:30:11 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	setup_and_execute_pipeline(t_split *commands, int cmd_count,
		t_shell *shell)
{
	int					**pipes;
	pid_t				*pids;
	t_pipeline_context	pipeline_ctx;

	if (!setup_pipeline_resources(&commands, &pipes, &pids, cmd_count))
		return (1);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	pipeline_ctx.commands = commands;
	pipeline_ctx.pipes = pipes;
	pipeline_ctx.pids = pids;
	pipeline_ctx.cmd_count = cmd_count;
	pipeline_ctx.shell = shell;
	cmd_count = execute_pipeline_children(&pipeline_ctx);
	setup_signals();
	cleanup_pipeline_resources(commands, pipes, pids, pipeline_ctx.cmd_count);
	return (cmd_count);
}

int	execute_pipeline_with_redirections(t_split split, t_shell *shell)
{
	t_split	*commands;
	int		cmd_count;
	int		exit_status;

	split = process_parentheses_in_split(split, shell);
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

static void	redirect_fds(int input_fd, int output_fd, int stderr_fd)
{
	if (input_fd != STDIN_FILENO)
	{
		dup2(input_fd, STDIN_FILENO);
		close(input_fd);
	}
	if (output_fd != STDOUT_FILENO)
	{
		dup2(output_fd, STDOUT_FILENO);
		close(output_fd);
	}
	if (stderr_fd != STDERR_FILENO)
	{
		dup2(stderr_fd, STDERR_FILENO);
		close(stderr_fd);
	}
}

static void	write_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

static void	execute_child_command(char **args, t_shell *shell)
{
	char	*executable;

	if (is_builtin(args[0]))
		exit(execute_builtin(args, shell));
	executable = find_executable(args[0], shell);
	if (!executable)
	{
		write_error_message(args[0], ": command not found\n");
		exit(127);
	}
	args = filter_empty_args(args);
	if (!args)
	{
		perror("filter_empty_args");
		exit(127);
	}
	execve(executable, args, shell->envp);
	perror("execve");
	free_args(args);
	exit(127);
}

void	execute_pipe_child_with_redirections(t_split cmd,
		t_pipe_child_context *ctx, t_shell *shell)
{
	char					**args;
	t_redir_fds				fds;
	t_pipe_setup_context	pipe_ctx;
	int						fd_values[3];

	fd_values[0] = STDIN_FILENO;
	fd_values[1] = STDOUT_FILENO;
	fd_values[2] = STDERR_FILENO;
	fds.input_fd = &fd_values[0];
	fds.output_fd = &fd_values[1];
	fds.stderr_fd = &fd_values[2];
	pipe_ctx.cmd_index = ctx->cmd_index;
	pipe_ctx.cmd_count = ctx->cmd_count;
	pipe_ctx.pipes = ctx->pipes;
	pipe_ctx.input_fd = &fd_values[0];
	pipe_ctx.output_fd = &fd_values[1];
	setup_pipe_fds(&pipe_ctx);
	args = parse_redirections(cmd, &fds, shell);
	if (!args)
		exit(1);
	redirect_fds(fd_values[0], fd_values[1], fd_values[2]);
	close_all_pipes(ctx->pipes, ctx->cmd_count);
	setup_child_signals();
	process_and_check_args(args, shell);
	execute_child_command(args, shell);
}
