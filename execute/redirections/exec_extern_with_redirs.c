/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_extern_with_redirs.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:13 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 17:15:07 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	close_parent_fds(int input_fd, int output_fd, int stderr_fd)
{
	if (input_fd != STDIN_FILENO)
		close(input_fd);
	if (output_fd != STDOUT_FILENO)
		close(output_fd);
	if (stderr_fd != STDERR_FILENO)
		close(stderr_fd);
}

static int	wait_for_child_exit(pid_t pid)
{
	int	status;
	int	exit_status;

	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		exit_status = 128 + WTERMSIG(status);
	else
		exit_status = 1;
	return (exit_status);
}

static void	child_setup_and_exec(t_redir_exec_ctx *ctx, char *executable)
{
	setup_child_signals();
	if (ctx->input_fd != STDIN_FILENO)
	{
		dup2(ctx->input_fd, STDIN_FILENO);
		close(ctx->input_fd);
	}
	if (ctx->output_fd != STDOUT_FILENO)
	{
		dup2(ctx->output_fd, STDOUT_FILENO);
		close(ctx->output_fd);
	}
	if (ctx->stderr_fd != STDERR_FILENO)
	{
		dup2(ctx->stderr_fd, STDERR_FILENO);
		close(ctx->stderr_fd);
	}
	execve(executable, ctx->args, ctx->shell->envp);
	perror("execve");
	if (ctx->args)
		free_args(ctx->args);
	if (ctx->shell)
		free_env(ctx->shell);
	exit(127);
}

int	execute_external_with_redirect(t_redir_exec_ctx *ctx)
{
	char		*executable;
	pid_t		pid;

	executable = find_executable(ctx->args[0], ctx->shell);
	if (!executable)
	{
		write(STDERR_FILENO, ctx->args[0], ft_strlen(ctx->args[0]));
		write(STDERR_FILENO, ": command not found\n", 20);
		return (127);
	}
	if (ft_strncmp(executable, "__NOT_DIRECTORY__", 17) == 0)
	{
		write_notdir(ctx->args[0]);
		free(executable);
		return (126);
	}
	pid = fork();
	if (pid == 0)
		child_setup_and_exec(ctx, executable);
	close_parent_fds(ctx->input_fd, ctx->output_fd, ctx->stderr_fd);
	free(executable);
	return (wait_for_child_exit(pid));
}
