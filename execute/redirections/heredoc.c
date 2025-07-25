/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:53:10 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 15:36:54 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

void	write_line_to_pipe(int pipe_fd, char *line)
{
	write(pipe_fd, line, ft_strlen(line));
	write(pipe_fd, "\n", 1);
}

int	handle_here_doc(int *pipe_fd, t_shell *shell, int should_expand,
		t_heredoc_params *params)
{
	pid_t	pid;

	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		return (1);
	}
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (1);
	}
	if (pid == 0)
	{
		execute_heredoc_child(params->delimiter, pipe_fd[1],
			shell, should_expand);
		close(pipe_fd[0]);
		free_heredoc(shell, params);
		exit(EXIT_SUCCESS);
	}
	return (handle_heredoc_parent(pid, pipe_fd));
}
