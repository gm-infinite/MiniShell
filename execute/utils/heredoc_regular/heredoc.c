/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:27:58 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 18:54:41 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../main/minishell.h"

static int	handle_heredoc_parent(pid_t pid, int *pipe_fd)
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

static void	restore_signals(void (*old_sigint)(int), void (*old_sigquit)(int))
{
	signal(SIGINT, old_sigint);
	signal(SIGQUIT, old_sigquit);
}

static int	handle_fork_error(int *pipe_fd, void (*old_sigint)(int),
		void (*old_sigquit)(int))
{
	perror("fork");
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	restore_signals(old_sigint, old_sigquit);
	return (1);
}

int	handle_here_doc(int *pipe_fd, t_shell *shell, int should_expand,
					t_heredoc_params *params)
{
	pid_t	pid;
	int		ret;
	void	(*old_sigint)(int);
	void	(*old_sigquit)(int);

	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		return (1);
	}
	old_sigint = signal(SIGINT, SIG_IGN);
	old_sigquit = signal(SIGQUIT, SIG_IGN);
	pid = fork();
	if (pid < 0)
		return (handle_fork_error(pipe_fd, old_sigint, old_sigquit));
	if (pid == 0)
		exec_hdoc_chld(params->delimiter, pipe_fd[1], shell, should_expand);
	ret = handle_heredoc_parent(pid, pipe_fd);
	if (ret == -1)
		write(STDOUT_FILENO, "\n", 1);
	restore_signals(old_sigint, old_sigquit);
	return (ret);
}

int	handle_here_document(char *processed_delimiter, t_redir_fds *fds,
		t_shell *shell, t_heredoc_params *params)
{
	int	here_doc_pipe[2];
	int	should_expand;
	int	ret;

	should_expand = !delimiter_was_quoted(params->delimiter);
	params->delimiter = processed_delimiter;
	ret = handle_here_doc(here_doc_pipe, shell, should_expand, params);
	if (ret != 0)
		return (-1);
	*fds->input_fd = here_doc_pipe[0];
	return (0);
}
