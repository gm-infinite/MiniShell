/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_with_redirs_2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:33:52 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 19:03:17 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

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
