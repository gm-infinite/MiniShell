/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:50:25 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:30:11 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	execute_pipeline_with_redirections(t_split split, t_shell *shell)
{
	t_split	*commands;
	int		cmd_count;
	int		**pipes;
	pid_t	*pids;
	int		exit_status;

	split = process_parentheses_in_split(split, shell);
	commands = split_by_pipes(split, &cmd_count);
	if (!commands)
		return (1);
	if (cmd_count == 1)
	{
		exit_status = execute_with_redirections(commands[0], shell);
		free(commands);
		return (exit_status);
	}
	if (!setup_pipeline_resources(&commands, &pipes, &pids, cmd_count))
		return (1);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	exit_status = execute_pipeline_children(commands, pipes, pids, cmd_count,
			shell);
	setup_signals();
	cleanup_pipeline_resources(commands, pipes, pids, cmd_count);
	return (exit_status);
}

static void	redirect_fds(int input_fd, int output_fd, int stderr_fd)
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

static void	execute_child_command(char **args, t_shell *shell)
{
	char	*executable;

	if (is_builtin(args[0]))
		exit(execute_builtin(args, shell));
	executable = find_executable(args[0], shell);
	if (!executable)
	{
		fprintf(stderr, "%s: command not found\n", args[0]);
		exit(127);
	}
	args = filter_empty_args(args);
	if (!args)
	{
		perror("filter_empty_args");
		exit(127);
	}
	execve(executable, args, shell->envp);
	perror("execve");
	free_args(args);
	exit(127);
}

void	execute_pipe_child_with_redirections(t_split cmd, int cmd_index,
		int **pipes, int cmd_count, t_shell *shell)
{
	char	**args;
	int		input_fd;
	int		output_fd;
	int		stderr_fd;
	int		i;

	input_fd = STDIN_FILENO;
	output_fd = STDOUT_FILENO;
	stderr_fd = STDERR_FILENO;
	setup_pipe_fds(cmd_index, cmd_count, pipes, &input_fd, &output_fd);
	args = parse_redirections(cmd, &input_fd, &output_fd, &stderr_fd, shell);
	if (!args)
		exit(1);
	redirect_fds(input_fd, output_fd, stderr_fd);
	close_all_pipes(pipes, cmd_count);
	setup_child_signals();
	if (!args[0])
	{
		free_args(args);
		exit(0);
	}
	i = 0;
	while (args[i])
	{
		args[i] = expand_variables_quoted(args[i], shell);
		i++;
	}
	compact_args(args);
	process_args_quotes(args, shell);
	if (!args[0])
	{
		free_args(args);
		exit(0);
	}
	if (args[0][0] == '\0')
	{
		write(STDERR_FILENO, ": command not found\n", 20);
		free_args(args);
		exit(127);
	}
	execute_child_command(args, shell);
}
