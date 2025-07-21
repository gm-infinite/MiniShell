/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe_command.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 22:24:01 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:31:52 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	expand_command_args(char **args, t_shell *shell)
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
	compact_args(args);
}

static void	execute_child_process(char **args, char *executable, t_shell *shell)
{
	char	**filtered_args;

	filtered_args = filter_empty_args(args);
	if (!filtered_args)
	{
		perror("filter_empty_args");
		exit(127);
	}
	if (execve(executable, filtered_args, shell->envp) == -1)
	{
		perror("execve");
		free_args(filtered_args);
		exit(127);
	}
}

static int	handle_builtin_command(char **args, t_pipe_context *ctx,
							t_shell *shell)
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

static int	handle_external_command(char **args, t_pipe_context *ctx,
							t_shell *shell)
{
	char	*executable;
	pid_t	pid;

	executable = find_executable(args[0], shell);
	if (!executable)
	{
		fprintf(stderr, "%s: command not found\n", args[0]);
		return (-1);
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

int	execute_pipe_command(t_split cmd, t_pipe_context *ctx, t_shell *shell)
{
	char	**args;
	pid_t	pid;

	if (cmd.size == 0)
		return (1);
	args = split_to_args(cmd);
	if (!args)
		return (1);
	expand_command_args(args, shell);
	if (!args[0])
	{
		free_args(args);
		return (0);
	}
	if (is_builtin(args[0]))
		pid = handle_builtin_command(args, ctx, shell);
	else
		pid = handle_external_command(args, ctx, shell);
	free_args(args);
	return (pid);
}
