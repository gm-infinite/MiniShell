/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_new.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 21:40:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/18 21:41:43 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_new.h"

t_split	process_parentheses_in_split(t_split cmd, t_shell *shell)
{
	int	i = 0;
	int	has_parens = 0;
	
	(void)shell;
	
	while (i < cmd.size)
	{
		if (ft_strchr(cmd.start[i], '(') || ft_strchr(cmd.start[i], ')'))
		{
			has_parens = 1;
			break;
		}
		i++;
	}
	
	if (has_parens)
		cut_out_par(&cmd);
	
	return (cmd);
}

static int	execute_external_command(char **args, t_shell *shell)
{
	char	*executable = find_executable(args[0], shell);
	pid_t	pid;
	int		status;
	
	if (!executable)
	{
		printf("%s: command not found\n", args[0]);
		return (127);
	}
	
	pid = fork();
	if (pid == 0)
	{
		setup_child_signals();
		if (execve(executable, args, shell->envp) == -1)
		{
			perror("execve");
			exit(127);
		}
	}
	else if (pid > 0)
	{
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		waitpid(pid, &status, 0);
		setup_signals();
		free(executable);
		return (WIFEXITED(status) ? WEXITSTATUS(status) : 1);
	}
	
	free(executable);
	return (1);
}

int	execute_single_command(char **args, t_shell *shell)
{
	if (!args || !args[0])
		return (1);
	
	process_args_quotes(args, shell);
	compact_args(args);
	
	if (!args[0])
		return (0);
	
	if (is_builtin(args[0]))
		return (execute_builtin(args, shell));
	
	return (execute_external_command(args, shell));
}

static void	setup_pipe_child(int cmd_index, int **pipes, int cmd_count, int input_fd, int output_fd)
{
	int	i = 0;
	
	(void)cmd_index; // Mark as unused
	
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
	
	while (i < cmd_count - 1)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
	
	setup_child_signals();
}

static int	execute_pipeline_segment(t_split cmd, int cmd_index, int **pipes, int cmd_count, t_shell *shell)
{
	char	**args;
	int		input_fd = STDIN_FILENO;
	int		output_fd = STDOUT_FILENO;
	pid_t	pid;
	
	if (cmd_index > 0)
		input_fd = pipes[cmd_index - 1][0];
	if (cmd_index < cmd_count - 1)
		output_fd = pipes[cmd_index][1];
	
	cmd = process_parentheses_in_split(cmd, shell);
	args = split_to_args(cmd);
	if (!args || !args[0])
		return (1);
	
	if (has_redirections(cmd))
		return (execute_with_redirections(cmd, shell));
	
	pid = fork();
	if (pid == 0)
	{
		setup_pipe_child(cmd_index, pipes, cmd_count, input_fd, output_fd);
		
		process_args_quotes(args, shell);
		compact_args(args);
		
		if (is_builtin(args[0]))
			exit(execute_builtin(args, shell));
		
		char *executable = find_executable(args[0], shell);
		if (!executable)
		{
			printf("%s: command not found\n", args[0]);
			exit(127);
		}
		
		execve(executable, args, shell->envp);
		perror("execve");
		exit(127);
	}
	
	free_args(args);
	return (pid);
}

int	execute_pipeline(t_split split, t_shell *shell)
{
	t_split	*commands;
	int		cmd_count;
	int		**pipes;
	pid_t	*pids;
	int		i = 0;
	int		status;
	int		exit_status = 0;
	
	commands = split_by_pipes(split, &cmd_count);
	if (!commands)
		return (1);
	
	if (cmd_count == 1)
	{
		char **args = split_to_args(commands[0]);
		exit_status = execute_single_command(args, shell);
		free_args(args);
		free(commands);
		return (exit_status);
	}
	
	pipes = malloc(sizeof(int *) * (cmd_count - 1));
	pids = malloc(sizeof(pid_t) * cmd_count);
	
	while (i < cmd_count - 1)
	{
		pipes[i] = malloc(sizeof(int) * 2);
		pipe(pipes[i]);
		i++;
	}
	
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	
	i = 0;
	while (i < cmd_count)
	{
		pids[i] = execute_pipeline_segment(commands[i], i, pipes, cmd_count, shell);
		i++;
	}
	
	i = 0;
	while (i < cmd_count - 1)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		free(pipes[i]);
		i++;
	}
	
	i = 0;
	while (i < cmd_count)
	{
		waitpid(pids[i], &status, 0);
		if (i == cmd_count - 1)
			exit_status = WIFEXITED(status) ? WEXITSTATUS(status) : 1;
		i++;
	}
	
	setup_signals();
	free(pipes);
	free(pids);
	free(commands);
	return (exit_status);
}

int	execute_command(t_split split, t_shell *shell)
{
	char	**args;
	int		exit_status;
	
	if (split.size == 0)
		return (0);
	
	if (count_pipes(split) > 0)
		return (execute_pipeline(split, shell));
	
	if (has_redirections(split))
		return (execute_with_redirections(split, shell));
	
	args = split_to_args(split);
	exit_status = execute_single_command(args, shell);
	free_args(args);
	
	return (exit_status);
}
