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

static char	**process_argument_expansion(char **args, t_shell *shell)
{
	char	*expanded;
	char	*reconstructed;
	char	*wildcard_expanded;
	t_split	new_split;
	int		i;

	// First pass: expand variables in each argument
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

	// Reconstruct the command string for wildcard expansion
	reconstructed = NULL;
	i = 0;
	while (args[i])
	{
		if (reconstructed == NULL)
			reconstructed = ft_strdup(args[i]);
		else
		{
			char *temp = ft_strjoin(reconstructed, " ");
			free(reconstructed);
			reconstructed = temp;
			temp = ft_strjoin(reconstructed, args[i]);
			free(reconstructed);
			reconstructed = temp;
		}
		i++;
	}

	if (!reconstructed)
	{
		process_args_quotes(args, shell);
		return (args);
	}

	// Apply wildcard expansion
	wildcard_expanded = wildcard_input_modify(reconstructed, shell);
	if (wildcard_expanded && wildcard_expanded != reconstructed)
	{
		free(reconstructed);
		reconstructed = wildcard_expanded;
	}

	// Split again with wildcard expansion applied
	new_split = create_split_str(reconstructed);
	free(reconstructed);

	if (new_split.size == 0)
	{
		free_split(&new_split);
		process_args_quotes(args, shell);
		return (args);
	}

	// Free old args and create new expanded args
	free_args(args);
	args = split_to_args(new_split);
	free_split(&new_split);

	if (args)
		process_args_quotes(args, shell);

	return (args);
}

static void	write_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

static void	execute_external_command(char **args, t_shell *shell)
{
	char	*executable;

	executable = find_executable(args[0], shell);
	if (!executable)
	{
		write_error_message(args[0], ": command not found\n");
		exit(127);
	}
	execve(executable, args, shell->envp);
	perror("execve");
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
	args = process_argument_expansion(args, shell);
	if (!args || !args[0])
		exit(0);
	if (is_builtin(args[0]))
		exit(execute_builtin(args, shell));
	else
		execute_external_command(args, shell);
}
