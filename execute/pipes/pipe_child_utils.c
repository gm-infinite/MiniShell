/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_child_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 00:00:00 by gm-infinite       #+#    #+#             */
/*   Updated: 2025/07/23 17:43:28 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

void	setup_and_execute_child(t_process_pipeline_context *proc_ctx, int i)
{
	t_pipe_child_context	ctx;

	ctx.pipes = proc_ctx->pipes;
	ctx.cmd_index = i;
	ctx.cmd_count = proc_ctx->cmd_count;
	execute_pipe_child(proc_ctx->commands[i], &ctx, proc_ctx->shell, proc_ctx->commands, proc_ctx->pids);
	free_child_pipeline_memory(NULL, proc_ctx->shell, proc_ctx->commands,
		proc_ctx->pipes, proc_ctx->pids, proc_ctx->cmd_count);
	exit(127);
}

void	setup_pipeline_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}
