/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:53:10 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/16 20:58:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

/**
 * - Returns pipe read end file descriptor for stdin redirection.
 * - This function modifies pipe_fd array (the one passed as argument) to return file descriptors.
 * - The caller is responsible for closing read end after use!
 * - Write end is closed by function, read end left open for caller.
 * - returns 0 on success, 1 on failure (pipe creation error)
 */

int	handle_here_doc(char *delimiter, int *pipe_fd)
{
	char	*line;      // Input line buffer from readline()

	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		return (1);
	}
	while (1)
	{
		line = readline("> ");
		if (!line)
			break;
		if (ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1) == 0)
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
