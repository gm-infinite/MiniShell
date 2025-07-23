/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_child_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 00:00:00 by gm-infinite       #+#    #+#             */
/*   Updated: 2025/07/23 19:32:33 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

void	setup_and_execute_child(t_process_pipeline_context *proc_ctx, int i)
{
	t_pipe_child_context	ctx;
	t_pipe_child_params		params;

	ctx.pipes = proc_ctx->pipes;
	ctx.cmd_index = i;
	ctx.cmd_count = proc_ctx->cmd_count;
	params.cmd = proc_ctx->commands[i];
	params.ctx = &ctx;
	params.shell = proc_ctx->shell;
	params.commands = proc_ctx->commands;
	params.pids = proc_ctx->pids;
	execute_pipe_child(&params);
	{
		t_pipeline_cleanup cleanup = {proc_ctx->commands, proc_ctx->pipes, proc_ctx->pids, proc_ctx->cmd_count};
		free_child_pipeline_memory(NULL, proc_ctx->shell, &cleanup);
	}
	exit(127);
}

void	setup_pipeline_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}
