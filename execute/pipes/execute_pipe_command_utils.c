/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe_command_utils.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 22:24:01 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:31:52 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	write_pipe_cmd_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

void	expand_command_args(char **args, t_shell *shell)
{
	char	*expanded;
	int		i;

	i = 0;
	while (args[i])
	{
		expanded = expand_variables(args[i], shell);
		if (expanded && expanded != args[i])
		{
			free(args[i]);
			args[i] = expanded;
		}
		i++;
	}
}

static void	execute_child_process(char **args, char *executable, t_shell *shell)
{
	if (execve(executable, args, shell->envp) == -1)
	{
		perror("execve");
		exit(127);
	}
}

int	handle_builtin_command(char **args, t_pipe_context *ctx, t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		setup_child_signals();
		setup_child_redirection(ctx);
		exit(execute_builtin(args, shell));
	}
	return (pid);
}

int	handle_external_command(char **args, t_pipe_context *ctx, t_shell *shell)
{
	char	*executable;
	pid_t	pid;

	executable = find_executable(args[0], shell);
	if (!executable)
	{
		write_pipe_cmd_error_message(args[0], ": command not found\n");
		exit(127);
	}
	pid = fork();
	if (pid == -1)
	{
		free(executable);
		return (-1);
	}
	if (pid == 0)
	{
		setup_child_signals();
		setup_child_redirection(ctx);
		execute_child_process(args, executable, shell);
	}
	free(executable);
	return (pid);
}
