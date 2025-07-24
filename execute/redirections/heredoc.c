/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:53:10 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 09:53:47 by kuzyilma         ###   ########.fr       */
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
			break ;
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

static void heredoc_sigint(int sig)
{
	(void)sig;
    rl_cleanup_after_signal();
    _exit(128 + SIGINT);
}

int handle_here_doc(char *delimiter,
                    int *pipe_fd,      /* [0]=read, [1]=write */
                    t_shell *shell,
                    int should_expand,
                    char **args)
{
    pid_t pid;
    int status;

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
        /* Child: disable Readline’s own handlers, install ours */
        rl_catch_signals = 0;
        signal(SIGINT, heredoc_sigint);
        signal(SIGQUIT, SIG_IGN);

        /* Do your existing loop to read lines & write to pipe_fd[1] */
        process_heredoc_loop(delimiter, pipe_fd[1], shell, should_expand);

        /* clean up and exit */
        close(pipe_fd[1]);
        close(pipe_fd[0]);
        free_heredoc_child_memory(args, shell);
        _exit(EXIT_SUCCESS);
    }

    /* Parent: close write‑end and wait for child */
    close(pipe_fd[1]);
    waitpid(pid, &status, 0);

    /* If child was killed by SIGINT or exited with 128+SIGINT → user ^Cʼd */
    if ((WIFSIGNALED(status) && WTERMSIG(status) == SIGINT) ||
        (WIFEXITED(status) && WEXITSTATUS(status) == 128 + SIGINT))
    {
        close(pipe_fd[0]);
        return (-1);
    }

    return (0);
}
