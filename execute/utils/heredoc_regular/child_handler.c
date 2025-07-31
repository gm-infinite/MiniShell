/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_handler.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:51:21 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 10:44:21 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../main/minishell.h"

static int	is_delimiter_match(char *line, char *delimiter)
{
	if (ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0
		&& ft_strlen(line) == ft_strlen(delimiter))
		return (1);
	return (0);
}

static int	process_heredoc_loop(char *delimiter, int pipe_fd, t_shell *shell,
		int should_expand)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (line == NULL || !line || *line == '\0')
		{
			write(STDERR_FILENO, "minishell: warning: here-document delimited"
				" by end-of-file (wanted `", 68);
			write(STDERR_FILENO, delimiter, ft_strlen(delimiter));
			write(STDERR_FILENO, "')\n", 3);
		}
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

void	exec_hdoc_chld(char *delimiter, int pipe_fd,
		t_shell *shell, int should_expand)
{
	rl_catch_signals = 0;
	signal(SIGINT, heredoc_sigint);
	signal(SIGQUIT, SIG_IGN);
	process_heredoc_loop(delimiter, pipe_fd, shell, should_expand);
	close(pipe_fd);
}
