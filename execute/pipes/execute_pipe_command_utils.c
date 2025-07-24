/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe_command_utils.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 22:24:01 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 20:53:12 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	write_pipe_cmd_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

static void	execute_child_process(char **args, char *executable, t_shell *shell)
{
	if (execve(executable, args, shell->envp) == -1)
	{
		perror("execve");
		exit(127);
	}
}

static int	handle_executable_errors(char **args, char *executable)
{
	if (!executable)
	{
		write_pipe_cmd_error_message(args[0], ": command not found\n");
		exit(127);
	}
	if (ft_strncmp(executable, "__NOT_DIRECTORY__", 17) == 0)
	{
		write_not_directory_error(args[0]);
		free(executable);
		exit(126);
	}
	return (0);
}

static int	create_child_process(char **args, t_pipe_context *ctx,
		t_shell *shell, char *executable)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		setup_child_signals();
		setup_child_redirection(ctx);
		execute_child_process(args, executable, shell);
	}
	return (pid);
}

int	handle_external_command(char **args, t_pipe_context *ctx, t_shell *shell)
{
	char	*executable;
	int		pid;

	executable = find_executable(args[0], shell);
	handle_executable_errors(args, executable);
	pid = create_child_process(args, ctx, shell, executable);
	free(executable);
	return (pid);
}
