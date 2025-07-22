/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:53:10 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:53:36 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"
#include "../../main/get_next_line.h"

int	handle_here_doc(char *delimiter, int *pipe_fd)
{
	char	*line;

	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		return (1);
	}
	while (1)
	{
		if (isatty(fileno(stdin)))
			line = readline("> ");
		else
		{
			line = get_next_line(fileno(stdin));
			if (line)
			{
				char *trimmed = ft_strtrim(line, "\n");
				free(line);
				line = trimmed;
			}
		}
		if (!line)
		{
			// EOF encountered before delimiter was found - show warning like bash
			write(STDERR_FILENO, "minishell: warning: here-document delimited by end-of-file (wanted `", 68);
			write(STDERR_FILENO, delimiter, ft_strlen(delimiter));
			write(STDERR_FILENO, "')\n", 3);
			break ;
		}
		if (ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0
			&& ft_strlen(line) == ft_strlen(delimiter))
		{
			free(line);
			break ;
		}
		write(pipe_fd[1], line, ft_strlen(line));
		write(pipe_fd[1], "\n", 1);
		free(line);
	}
	close(pipe_fd[1]);
	return (0);
}
