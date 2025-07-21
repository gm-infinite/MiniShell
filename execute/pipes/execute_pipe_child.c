/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe_child.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 22:25:13 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:30:44 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	setup_pipe_redirection(int cmd_index, int cmd_count, int **pipes)
{
	if (cmd_index > 0)
		dup2(pipes[cmd_index - 1][0], STDIN_FILENO);
	if (cmd_index < cmd_count - 1)
		dup2(pipes[cmd_index][1], STDOUT_FILENO);
}

static void	cleanup_pipe_descriptors(int **pipes, int cmd_count)
{
	int	i;

	i = 0;
	while (i < cmd_count - 1)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

static void	process_argument_expansion(char **args, t_shell *shell)
{
	char	*expanded;
	int		i;

	i = 0;
	while (args[i])
	{
		expanded = expand_variables_quoted(args[i], shell);
		if (expanded && expanded != args[i])
		{
			free(args[i]);
			args[i] = expanded;
		}
		i++;
	}
	compact_args(args);
	process_args_quotes(args, shell);
}

static void	write_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

static void	execute_external_command(char **args, t_shell *shell)
{
	char	*executable;
	char	**filtered_args;

	executable = find_executable(args[0], shell);
	if (!executable)
	{
		write_error_message(args[0], ": command not found\n");
		exit(127);
	}
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

void	execute_pipe_child(t_split cmd, t_pipe_child_context *ctx,
		t_shell *shell)
{
	char	**args;

	setup_pipe_redirection(ctx->cmd_index, ctx->cmd_count, ctx->pipes);
	cleanup_pipe_descriptors(ctx->pipes, ctx->cmd_count);
	setup_child_signals();
	if (has_parentheses_in_split(cmd))
	{
		parser_and_or(shell, cmd);
		exit(shell->past_exit_status);
	}
	args = split_to_args(cmd);
	if (!args || !args[0])
		exit(1);
	process_argument_expansion(args, shell);
	if (!args[0])
		exit(0);
	if (is_builtin(args[0]))
		exit(execute_builtin(args, shell));
	else
		execute_external_command(args, shell);
}
