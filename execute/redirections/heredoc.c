/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:53:10 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/23 11:34:19 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"
#include "../../main/get_next_line.h"

static void	write_line_to_pipe(int pipe_fd, char *line)
{
	write(pipe_fd, line, ft_strlen(line));
	write(pipe_fd, "\n", 1);
}

static int	process_heredoc_loop(char *delimiter, int pipe_fd, t_shell *shell,
		int should_expand)
{
	char	*line;
	char	*expanded_line;

	while (1)
	{
		line = read_heredoc_input_line();
		if (!line)
		{
			write_heredoc_warning_message(delimiter);
			break ;
		}
		if (is_delimiter_match(line, delimiter))
		{
			free(line);
			break ;
		}
		if (should_expand && shell)
		{
			expanded_line = expand_variables(line, shell);
			if (expanded_line)
			{
				write_line_to_pipe(pipe_fd, expanded_line);
				free(expanded_line);
			}
			else
				write_line_to_pipe(pipe_fd, line);
		}
		else
			write_line_to_pipe(pipe_fd, line);
		free(line);
	}
	return (0);
}

int	handle_here_doc(char *delimiter, int *pipe_fd, t_shell *shell, 
		int should_expand)
{
	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		return (1);
	}
	process_heredoc_loop(delimiter, pipe_fd[1], shell, should_expand);
	close(pipe_fd[1]);
	return (0);
}
