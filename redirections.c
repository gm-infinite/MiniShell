/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 21:45:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/18 21:41:43 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_new.h"

int	handle_here_doc(char *delimiter, int *pipe_fd)
{
	char	*line;
	int		temp_pipe[2];
	
	if (pipe(temp_pipe) == -1)
		return (-1);
	
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1) == 0)
		{
			if (line)
				free(line);
			break;
		}
		write(temp_pipe[1], line, ft_strlen(line));
		write(temp_pipe[1], "\n", 1);
		free(line);
	}
	
	close(temp_pipe[1]);
	pipe_fd[0] = temp_pipe[0];
	pipe_fd[1] = -1;
	return (0);
}

static int	open_file(char *filename, char *mode)
{
	int	fd;
	int	flags = 0;
	
	if (ft_strncmp(mode, "<<", 3) == 0)
	{
		int pipe_fd[2];
		if (handle_here_doc(filename, pipe_fd) == -1)
			return (-1);
		return (pipe_fd[0]);
	}
	
	if (ft_strncmp(mode, "<", 2) == 0)
		flags = O_RDONLY;
	else if (ft_strncmp(mode, ">>", 3) == 0)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else if (ft_strncmp(mode, ">", 2) == 0)
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	
	fd = open(filename, flags, 0644);
	if (fd == -1)
		perror(filename);
	
	return (fd);
}

char	**parse_redirections(t_split split, int *input_fd, int *output_fd, int *stderr_fd)
{
	char	**clean_args = malloc(sizeof(char *) * (split.size + 1));
	int		clean_count = 0;
	int		i = 0;
	
	*input_fd = STDIN_FILENO;
	*output_fd = STDOUT_FILENO;
	*stderr_fd = STDERR_FILENO;
	
	if (!clean_args)
		return (NULL);
	
	while (i < split.size)
	{
		if (is_redirection(split.start[i]))
		{
			if (i + 1 >= split.size)
			{
				printf("syntax error near unexpected token `newline'\n");
				free(clean_args);
				return (NULL);
			}
			
			int fd = open_file(split.start[i + 1], split.start[i]);
			if (fd == -1)
			{
				free(clean_args);
				return (NULL);
			}
			
			if (ft_strchr(split.start[i], '<'))
				*input_fd = fd;
			else
				*output_fd = fd;
			
			i += 2;
		}
		else
		{
			clean_args[clean_count] = ft_strdup(split.start[i]);
			clean_count++;
			i++;
		}
	}
	
	clean_args[clean_count] = NULL;
	return (clean_args);
}

int	execute_with_redirections(t_split split, t_shell *shell)
{
	int		input_fd, output_fd, stderr_fd;
	char	**args;
	int		exit_status;
	int		saved_stdin = dup(STDIN_FILENO);
	int		saved_stdout = dup(STDOUT_FILENO);
	
	args = parse_redirections(split, &input_fd, &output_fd, &stderr_fd);
	if (!args)
		return (1);
	
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
	
	exit_status = execute_single_command(args, shell);
	
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
	
	free_args(args);
	return (exit_status);
}
