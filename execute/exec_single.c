/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_single.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 16:20:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 16:52:01 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	validate_and_process_args(char **args, t_shell *shell)
{
	int	i;

	if (!args || !args[0])
		return (1);
	i = 0;
	while (args[i])
	{
		args[i] = expand_variables_quoted(args[i], shell);
		i++;
	}
	compact_args(args);
	process_args_quotes(args, shell);
	if (!args[0])
		return (0);
	if (args[0][0] == '\0')
	{
		write(STDERR_FILENO, ": command not found\n", 20);
		return (127);
	}
	return (-1);
}

static int	validate_executable(char *cmd, char *executable)
{
	struct stat	file_stat;

	if (stat(executable, &file_stat) == 0)
	{
		if (S_ISDIR(file_stat.st_mode))
		{
			write(STDERR_FILENO, cmd, ft_strlen(cmd));
			write(STDERR_FILENO, ": Is a directory\n", 17);
			return (126);
		}
		if (access(executable, X_OK) != 0)
		{
			write(STDERR_FILENO, cmd, ft_strlen(cmd));
			write(STDERR_FILENO, ": Permission denied\n", 20);
			return (126);
		}
	}
	return (0);
}

static int	execute_child_process(char **args, char *executable, t_shell *shell)
{
	char	**filtered_args;

	setup_child_signals();
	filtered_args = filter_empty_args(args);
	if (!filtered_args)
	{
		perror("filter_empty_args");
		free(executable);
		exit(127);
	}
	if (execve(executable, filtered_args, shell->envp) == -1)
	{
		perror("execve");
		free(executable);
		free_args(filtered_args);
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
			write(STDOUT_FILENO, "Quit: 3\n", 8);
		return (128 + WTERMSIG(status));
	}
	return (0);
}

int	execute_single_command(char **args, t_shell *shell)
{
	pid_t	pid;
	char	*executable;
	int		validation_result;

	validation_result = validate_and_process_args(args, shell);
	if (validation_result != -1)
		return (validation_result);
	if (is_builtin(args[0]))
		return (execute_builtin(args, shell));
	executable = find_executable(args[0], shell);
	if (!executable)
	{
		write(STDERR_FILENO, args[0], ft_strlen(args[0]));
		if (ft_strchr(args[0], '/'))
			write(STDERR_FILENO, ": No such file or directory\n", 28);
		else
			write(STDERR_FILENO, ": command not found\n", 20);
		return (127);
	}
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
