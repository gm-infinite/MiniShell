/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_with_redirs.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:33:52 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 12:50:36 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

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

static int	setup_and_execute_pipeline(t_split *commands, int cmd_count,
	t_shell *shell)
{
	int					**pipes;
	pid_t				*pids;
	t_pipe_ctx			pipeline_ctx;

	if (!setup_pipe(&commands, &pipes, &pids, cmd_count))
		return (1);
	pipeline_ctx.commands = commands;
	pipeline_ctx.pipes = pipes;
	pipeline_ctx.pids = pids;
	pipeline_ctx.cmd_count = cmd_count;
	pipeline_ctx.shell = shell;
	void (*old_sigint)(int);
	void (*old_sigquit)(int);
	old_sigint = signal(SIGINT, SIG_IGN);
	old_sigquit = signal(SIGQUIT, SIG_IGN);
	cmd_count = exec_pipe_child(&pipeline_ctx);
	signal(SIGINT, old_sigint);
	signal(SIGQUIT, old_sigquit);
	setup_signals();
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
