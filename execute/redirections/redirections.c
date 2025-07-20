/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 14:15:09 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 16:52:01 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

void redirection_fail_procedure(int *input_fd, int *output_fd, int *stderr_fd)
{
	if (*input_fd != STDIN_FILENO && *input_fd != -1)
	{
		close(*input_fd);
		*input_fd = STDIN_FILENO;
	}
	if (*output_fd != STDOUT_FILENO && *output_fd != -1)
	{
		close(*output_fd);
		*output_fd = STDOUT_FILENO;
	}
	if (*stderr_fd != STDERR_FILENO && *stderr_fd != -1)
	{
		close(*stderr_fd);
		*stderr_fd = STDERR_FILENO;
	}
}

char	**parse_redirections(t_split split, int *input_fd, int *output_fd, int *stderr_fd, t_shell *shell)
{
	char	**args;
	char	**clean_args;
	int		i;
	int		clean_count;
	int		redirection_failed = 0;

	args = split_to_args(split);
	if (!args)
		return (NULL);
	clean_count = count_clean_args(args);
	clean_args = build_clean_args(args, clean_count);
	if (!clean_args)
	{
		free_args(args);
		return (NULL);
	}
	i = 0;
	while (args[i] && !redirection_failed)
	{
		if (is_redirection(args[i]))
		{
			if (process_single_redirection(args, i, input_fd,
					output_fd, stderr_fd, shell) == -1)
				redirection_failed = 1;
			i += 2;
		}
		else
			i++;
	}
	if (redirection_failed)
	{
		free_args(args);
		free_args(clean_args);
		redirection_fail_procedure(input_fd, output_fd, stderr_fd);
		return (NULL);
	}
	free_args(args);
	return (clean_args);
}

int	execute_with_redirections(t_split split, t_shell *shell)
{
	char	**args;
	int		input_fd = STDIN_FILENO;
	int		output_fd = STDOUT_FILENO;
	int		stderr_fd = STDERR_FILENO;
	int		saved_stdin = -1;
	int		saved_stdout = -1;
	int		saved_stderr = -1;
	int		exit_status;
	int		i;

	args = parse_redirections(split, &input_fd, &output_fd, &stderr_fd, shell);
	if (!args)
		return (1);
	if (!args[0])
	{
		free_args(args);
		if (input_fd != STDIN_FILENO)
			close(input_fd);
		if (output_fd != STDOUT_FILENO)
			close(output_fd);
		if (stderr_fd != STDERR_FILENO)
			close(stderr_fd);
		return (0);
	}
	i = 0;
	while (args[i])
	{
		char *expanded = expand_variables(args[i], shell);
		if (expanded)
		{
			free(args[i]);
			args[i] = expanded;
		}
		i++;
	}
	compact_args(args);
	process_args_quotes(args, shell);
	
	// Compact arguments to remove empty strings from variable expansion
	
	// Check for empty command after variable expansion and compaction
	if (!args[0])
	{
		// Close redirected file descriptors before returning
		if (input_fd != STDIN_FILENO)
			close(input_fd);
		if (output_fd != STDOUT_FILENO)
			close(output_fd);
		if (stderr_fd != STDERR_FILENO)
			close(stderr_fd);
		free_args(args);
		return (0);
	}
	
	// For built-in commands, redirect in current process
	if (is_builtin(args[0]))
	{
		// Save original stdin/stdout/stderr
		if (input_fd != STDIN_FILENO)
		{
			saved_stdin = dup(STDIN_FILENO);
			dup2(input_fd, STDIN_FILENO);
			close(input_fd);
		}
		if (output_fd != STDOUT_FILENO)
		{
			saved_stdout = dup(STDOUT_FILENO);
			dup2(output_fd, STDOUT_FILENO);
			close(output_fd);
		}
		if (stderr_fd != STDERR_FILENO)
		{
			saved_stderr = dup(STDERR_FILENO);
			dup2(stderr_fd, STDERR_FILENO);
			close(stderr_fd);
		}
		
		exit_status = execute_builtin(args, shell);
		if (saved_stdin != -1)
		{
			dup2(saved_stdin, STDIN_FILENO);
			close(saved_stdin);
		}
		if (saved_stdout != -1)
		{
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
		if (saved_stderr != -1)
		{
			dup2(saved_stderr, STDERR_FILENO);
			close(saved_stderr);
		}
	}
	else
	{
		if (args[0][0] == '\0')
		{
			write(STDERR_FILENO, ": command not found\n", 20);
			free_args(args);
			return (127);
		}
		char *executable = find_executable(args[0], shell);
		if (!executable)
		{
			write(STDERR_FILENO, args[0], ft_strlen(args[0]));
			write(STDERR_FILENO, ": command not found\n", 20);
			free_args(args);
			return (127);
		}
		
		pid_t pid = fork();
		if (pid == 0)
		{
			setup_child_signals();
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
			char **filtered_args = filter_empty_args(args);
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
		else
		{
			int status;
			if (input_fd != STDIN_FILENO)
				close(input_fd);
			if (output_fd != STDOUT_FILENO)
				close(output_fd);
			if (stderr_fd != STDERR_FILENO)
				close(stderr_fd);
			waitpid(pid, &status, 0);
			if (WIFEXITED(status))
				exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				exit_status = 128 + WTERMSIG(status);
			else
				exit_status = 1;
		}
		free(executable);
	}
	
	free_args(args);
	return (exit_status);
}
