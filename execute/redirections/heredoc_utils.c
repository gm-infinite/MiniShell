/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 13:07:53 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/24 13:19:19 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	process_and_write_line(char *line, int pipe_fd, t_shell *shell,
		int should_expand)
{
	char	*expanded_line;

	if (should_expand && shell)
	{
		expanded_line = expandvar(line, shell);
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
}

static int	process_heredoc_loop(char *delimiter, int pipe_fd, t_shell *shell,
		int should_expand)
{
	char	*line;

	while (1)
	{
		line = read_heredoc();
		if (!line || is_delimiter_match(line, delimiter))
		{
			if (line)
				free(line);
			break ;
		}
		process_and_write_line(line, pipe_fd, shell, should_expand);
		free(line);
	}
	return (0);
}

static void	heredoc_sigint(int sig)
{
	(void)sig;
	rl_cleanup_after_signal();
	exit(128 + SIGINT);
}

void	execute_heredoc_child(char *delimiter, int pipe_fd,
		t_shell *shell, int should_expand)
{
	rl_catch_signals = 0;
	signal(SIGINT, heredoc_sigint);
	signal(SIGQUIT, SIG_IGN);
	process_heredoc_loop(delimiter, pipe_fd, shell, should_expand);
	close(pipe_fd);
}

int	handle_heredoc_parent(pid_t pid, int *pipe_fd)
{
	int	status;

	close(pipe_fd[1]);
	waitpid(pid, &status, 0);
	if ((WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		|| (WIFEXITED(status) && WEXITSTATUS(status) == 128 + SIGINT))
	{
		close(pipe_fd[0]);
		return (-1);
	}
	return (0);
}
