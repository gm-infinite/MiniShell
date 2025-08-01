/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   p_w_r_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 16:41:24 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 19:00:06 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	fork_pipe_child(t_pipe_ctx *pipeline_ctx, int i)
{
	t_child_ctx				ctx;
	t_child_redir_params	params;

	pipeline_ctx->pids[i] = fork();
	if (pipeline_ctx->pids[i] == -1)
	{
		perror("fork");
		return (1);
	}
	else if (pipeline_ctx->pids[i] == 0)
	{
		ctx.pipes = pipeline_ctx->pipes;
		ctx.cmd_index = i;
		ctx.cmd_count = pipeline_ctx->cmd_count;
		ctx.heredoc_fds = pipeline_ctx->heredoc_fds;
		params.cmd = pipeline_ctx->commands[i];
		params.ctx = &ctx;
		params.shell = pipeline_ctx->shell;
		params.commands = pipeline_ctx->commands;
		params.pids = pipeline_ctx->pids;
		execute_child_redir(&params);
		exit(127);
	}
	return (0);
}

void	assign_params(t_heredoc_params *p, char **tokens, char *delim)
{
	p->delimiter = delim;
	p->args = tokens;
	p->clean_args = NULL;
}

int	fd_closer(int *fd)
{
	if (*fd >= 0)
	{
		close(*fd);
		*fd = -1;
	}
	return (0);
}
