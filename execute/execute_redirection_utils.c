/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirection_utils.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:50:25 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 21:08:24 by kuzyilma         ###   ########.fr       */
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

void	redirect_fds(int input_fd, int output_fd, int stderr_fd)
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

static void	execute_external_command(char **args, char *executable,
		t_shell *shell)
{
	execve(executable, args, shell->envp);
	perror("execve");
	free_child_memory(args, shell);
	exit(127);
}

void	execute_child_command(char **args, t_shell *shell)
{
	char	*executable;
	int		builtin_result;

	if (is_builtin(args[0]))
	{
		builtin_result = execute_builtin(args, shell);
		free_child_memory(args, shell);
		exit(builtin_result);
	}
	executable = find_executable(args[0], shell);
	handle_executable_error_and_exit(args, executable, shell);
	execute_external_command(args, executable, shell);
}

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
