/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_with_redirs.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:33:52 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 19:10:11 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static int	handle_single_heredoc(t_pipe_ctx *ctx, char **tokens, int j,
	int *pipe_fd)
{
	int					should_expand;
	char				*delim;
	t_heredoc_params	p;

	delim = process_heredoc_delimiter(tokens[j + 1]);
	if (!delim)
		return (-1);
	should_expand = !delimiter_was_quoted(tokens[j + 1]);
	assign_params(&p, tokens, delim);
	if (handle_here_doc(pipe_fd, ctx->shell, should_expand, &p) != 0)
	{
		free(delim);
		return (-1);
	}
	free(delim);
	return (0);
}

static int	process_command_heredocs(t_pipe_ctx *ctx, int cmd_index,
	int *pipe_fd)
{
	char	**tokens;
	int		sz;
	int		fd;
	int		j;

	tokens = ctx->commands[cmd_index].start;
	sz = ctx->commands[cmd_index].size;
	fd = -1;
	j = -1;
	while (++j < sz)
	{
		if (is_redirection(tokens[j]) == 1)
		{
			fd_closer(&fd);
			if (handle_single_heredoc(ctx, tokens, j, pipe_fd) != 0)
			{
				fd_closer(&fd);
				return (-1);
			}
			fd = pipe_fd[0];
		}
	}
	ctx->heredoc_fds[cmd_index] = fd;
	return (0);
}

int	preprocess_heredocs(t_pipe_ctx *ctx)
{
	int	i;
	int	pipe_fd[2];

	ctx->heredoc_fds = malloc(sizeof(int) * ctx->cmd_count);
	if (!ctx->heredoc_fds)
		return (0);
	i = -1;
	while (++i < ctx->cmd_count)
		ctx->heredoc_fds[i] = -1;
	i = -1;
	while (++i < ctx->cmd_count)
	{
		if (process_command_heredocs(ctx, i, pipe_fd) != 0)
		{
			free(ctx->heredoc_fds);
			ctx->heredoc_fds = NULL;
			return (0);
		}
	}
	return (1);
}

static int	wait_for_others(pid_t *pids, int cmd_count)
{
	int	status;
	int	exit_status;
	int	i;

	exit_status = 0;
	i = -1;
	while (++i < cmd_count)
	{
		waitpid(pids[i], &status, 0);
		if (WIFEXITED(status))
			exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
		{
			exit_status = 128 + WTERMSIG(status);
			if (WTERMSIG(status) == SIGINT)
				write(1, "\n", 1);
		}
	}
	return (exit_status);
}

int	exec_pipe_child(t_pipe_ctx *pipeline_ctx)
{
	int	i;

	i = -1;
	while (++i < pipeline_ctx->cmd_count)
	{
		if (fork_pipe_child(pipeline_ctx, i) != 0)
			return (1);
	}
	close_all_pipes(pipeline_ctx->pipes, pipeline_ctx->cmd_count);
	return (wait_for_others(pipeline_ctx->pids, pipeline_ctx->cmd_count));
}
