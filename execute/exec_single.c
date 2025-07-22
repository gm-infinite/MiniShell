/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_single.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 16:20:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/22 17:53:14 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	execute_child_process(char **args, char *executable, t_shell *shell)
{
	setup_child_signals();
	if (execve(executable, args, shell->envp) == -1)
	{
		perror("execve");
		free(executable);
		exit(127);
	}
	return (0);
}

static int	handle_parent_process(pid_t pid, char *executable)
{
	int	status;

	free(executable);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	if (waitpid(pid, &status, 0) == -1)
	{
		perror("waitpid");
		setup_signals();
		return (1);
	}
	setup_signals();
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT)
			write(STDOUT_FILENO, "\n", 1);
		else if (WTERMSIG(status) == SIGQUIT)
			write(STDOUT_FILENO, "Quit (core dumped)\n", 20);
		return (128 + WTERMSIG(status));
	}
	return (0);
}

static int	execute_external_command(char **args, t_shell *shell)
{
	char	*executable;
	int		validation_result;
	pid_t	pid;

	executable = find_executable(args[0], shell);
	if (!executable)
		return (handle_executable_not_found(args));
	validation_result = validate_executable(args[0], executable);
	if (validation_result != 0)
	{
		free(executable);
		return (validation_result);
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(executable);
		return (1);
	}
	else if (pid == 0)
		return (execute_child_process(args, executable, shell));
	else
		return (handle_parent_process(pid, executable));
}

int	execute_single_command(char **args, t_shell *shell)
{
	int	validation_result;

	validation_result = validate_and_process_args(args, shell);
	if (validation_result != -1)
		return (validation_result);
	if (is_builtin(args[0]))
		return (execute_builtin(args, shell));
	return (execute_external_command(args, shell));
}
