/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_external.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:25:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 22:00:13 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	setup_child_redirections(int input_fd, int output_fd, int stderr_fd)
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

int	execute_external_with_redirect(char **args, int input_fd, int output_fd, 
								int stderr_fd, t_shell *shell)
{
	char		*executable;
	pid_t		pid;
	char		**filtered_args;

	executable = find_executable(args[0], shell);
	if (!executable)
	{
		write(STDERR_FILENO, args[0], ft_strlen(args[0]));
		write(STDERR_FILENO, ": command not found\n", 20);
		return (127);
	}
	pid = fork();
	if (pid == 0)
	{
		setup_child_signals();
		setup_child_redirections(input_fd, output_fd, stderr_fd);
		filtered_args = filter_empty_args(args);
		if (!filtered_args)
		{
			perror("filter_empty_args");
			exit(127);
		}
		execve(executable, filtered_args, shell->envp);
		perror("execve");
		free_args(filtered_args);
		exit(127);
	}
	close_parent_fds(input_fd, output_fd, stderr_fd);
	free(executable);
	return (wait_for_child_exit(pid));
}
