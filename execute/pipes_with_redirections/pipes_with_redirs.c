/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_with_redirs.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:33:52 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 18:51:31 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void assign_params(t_heredoc_params *p, char **tokens, char *delim)
{
	p->delimiter = delim;
	p->args = tokens;
	p->clean_args = NULL;
}

static int fd_closer(int *fd)
{
	if (*fd >= 0)
	{
		close(*fd);
		*fd = -1;
	}
	return (0);
}

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

static int	preprocess_heredocs(t_pipe_ctx *ctx)
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

static int	exec_pipe_child(t_pipe_ctx *pipeline_ctx)
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

static void	setup_pipeline_ctx(t_pipe_ctx *pipeline_ctx, t_split *commands,
	t_shell *shell, int cmd_count)
{
	pipeline_ctx->commands = commands;
	pipeline_ctx->shell = shell;
	pipeline_ctx->cmd_count = cmd_count;
	pipeline_ctx->heredoc_fds = NULL;
}

static void	close_heredoc_fds(t_pipe_ctx *pipeline_ctx)
{
	int	i;

	i = -1;
	while (++i < pipeline_ctx->cmd_count)
		if (pipeline_ctx->heredoc_fds[i] >= 0)
			close(pipeline_ctx->heredoc_fds[i]);
	free(pipeline_ctx->heredoc_fds);
	pipeline_ctx->heredoc_fds = NULL;
}

static int	setup_and_execute_pipeline(t_split *commands, int cmd_count,
	t_shell *shell)
{
	int			**pipes;
	pid_t		*pids;
	t_pipe_ctx	pipeline_ctx;
	void		(*old_sigint)(int);
	void		(*old_sigquit)(int);

	if (!setup_pipe(&commands, &pipes, &pids, cmd_count))
		return (1);
	setup_pipeline_ctx(&pipeline_ctx, commands, shell, cmd_count);
	pipeline_ctx.pipes = pipes;
	pipeline_ctx.pids = pids;
	if (!preprocess_heredocs(&pipeline_ctx))
		return (1);
	old_sigint = signal(SIGINT, SIG_IGN);
	old_sigquit = signal(SIGQUIT, SIG_IGN);
	cmd_count = exec_pipe_child(&pipeline_ctx);
	signal(SIGINT, old_sigint);
	signal(SIGQUIT, old_sigquit);
	setup_signals();
	if (pipeline_ctx.heredoc_fds)
		close_heredoc_fds(&pipeline_ctx);
	clean_pipe(commands, pipes, pids, pipeline_ctx.cmd_count);
	return (cmd_count);
}

int	execute_pipe_redir(t_split split, t_shell *shell)
{
	t_split	*commands;
	int		cmd_count;
	int		exit_status;

	commands = split_by_pipes(split, &cmd_count);
	if (!commands)
		return (1);
	if (cmd_count == 1)
	{
		exit_status = execute_with_redirections(commands[0], shell);
		free(commands);
		return (exit_status);
	}
	return (setup_and_execute_pipeline(commands, cmd_count, shell));
}
