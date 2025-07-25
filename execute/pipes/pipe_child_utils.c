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

void	setup_child(t_process_pipe_ctx *proc_ctx, int i)
{
	t_child_ctx		ctx;
	t_child_params	params;
	t_pipe_cleaner	cleanup;

	ctx.pipes = proc_ctx->pipes;
	ctx.cmd_index = i;
	ctx.cmd_count = proc_ctx->cmd_count;
	params.cmd = proc_ctx->commands[i];
	params.ctx = &ctx;
	params.shell = proc_ctx->shell;
	params.commands = proc_ctx->commands;
	params.pids = proc_ctx->pids;
	execute_pipe_child(&params);
	cleanup.commands = proc_ctx->commands;
	cleanup.pipes = proc_ctx->pipes;
	cleanup.pids = proc_ctx->pids;
	cleanup.cmd_count = proc_ctx->cmd_count;
	free_child_pipeline_memory(NULL, proc_ctx->shell, &cleanup);
	exit(127);
}

void	setup_pipe_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}
