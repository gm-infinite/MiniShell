/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirection_utils.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:50:25 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:30:11 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	setup_and_execute_pipeline(t_split *commands, int cmd_count,
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

void	write_exec_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

void	execute_child_command(char **args, t_shell *shell)
{
	char	*executable;

	if (is_builtin(args[0]))
		exit(execute_builtin(args, shell));
	executable = find_executable(args[0], shell);
	if (!executable)
	{
		write_exec_error_message(args[0], ": command not found\n");
		exit(127);
	}
	execve(executable, args, shell->envp);
	perror("execve");
	exit(127);
}
