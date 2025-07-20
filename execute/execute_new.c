/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_new.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:50:25 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 16:52:01 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	create_pipes_array(int ***pipes, int cmd_count)
{
	int	i;

	*pipes = malloc(sizeof(int *) * (cmd_count - 1));
	if (!*pipes)
		return (0);
	i = 0;
	while (i < cmd_count - 1)
	{
		(*pipes)[i] = malloc(sizeof(int) * 2);
		if (pipe((*pipes)[i]) == -1)
		{
			perror("pipe");
			while (--i >= 0)
			{
				close((*pipes)[i][0]);
				close((*pipes)[i][1]);
				free((*pipes)[i]);
			}
			free(*pipes);
			return (0);
		}
		i++;
	}
	return (1);
}

static void	cleanup_pipes(int **pipes, int cmd_count)
{
	int	i;

	i = 0;
	while (i < cmd_count - 1)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		free(pipes[i]);
		i++;
	}
	free(pipes);
}

static int	wait_for_children(pid_t *pids, int cmd_count)
{
	int	i;
	int	status;
	int	exit_status;

	exit_status = 0;
	i = 0;
	while (i < cmd_count)
	{
		if (pids[i] > 0)
		{
			waitpid(pids[i], &status, 0);
			if (i == cmd_count - 1)
			{
				if (WIFEXITED(status))
					exit_status = WEXITSTATUS(status);
				else if (WIFSIGNALED(status))
					exit_status = 128 + WTERMSIG(status);
			}
		}
		i++;
	}
	return (exit_status);
}

int	execute_pipeline_with_redirections(t_split split, t_shell *shell)
{
	t_split	*commands;
	int		cmd_count;
	int		**pipes;
	pid_t	*pids;
	int		i;
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
	if (!create_pipes_array(&pipes, cmd_count))
	{
		free(commands);
		return (1);
	}
	pids = malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
	{
		cleanup_pipes(pipes, cmd_count);
		free(commands);
		return (1);
	}
	i = 0;
	while (i < cmd_count)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			exit_status = 1;
			break ;
		}
		else if (pids[i] == 0)
		{
			execute_pipe_child_with_redirections(commands[i], i, pipes,
				cmd_count, shell);
			exit(127);
		}
		i++;
	}
	cleanup_pipes(pipes, cmd_count);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	exit_status = wait_for_children(pids, cmd_count);
	setup_signals();
	free(pids);
	free(commands);
	return (exit_status);
}

static void	setup_pipe_fds(int cmd_index, int cmd_count, int **pipes,
			int *input_fd, int *output_fd)
{
	if (cmd_index > 0)
		*input_fd = pipes[cmd_index - 1][0];
	if (cmd_index < cmd_count - 1)
		*output_fd = pipes[cmd_index][1];
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

static void	close_all_pipes(int **pipes, int cmd_count)
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

static void	execute_child_command(char **args, t_shell *shell)
{
	char	*executable;

	if (is_builtin(args[0]))
		exit(execute_builtin(args, shell));
	executable = find_executable(args[0], shell);
	if (!executable)
	{
		write(STDERR_FILENO, args[0], ft_strlen(args[0]));
		write(STDERR_FILENO, ": command not found\n", 20);
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
