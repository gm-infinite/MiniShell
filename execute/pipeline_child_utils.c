/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_child_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 20:17:42 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	preprocess_heredocs_in_pipeline(t_pipeline_context *pipeline_ctx)
{
	int		i;
	char	**args;

	i = 0;
	while (i < pipeline_ctx->cmd_count)
	{
		args = split_to_args(pipeline_ctx->commands[i]);
		if (!args)
		{
			i++;
			continue ;
		}
		if (process_command_redirections(args, pipeline_ctx, i) != 0)
		{
			free_args(args);
			return (1);
		}
		free_args(args);
		i++;
	}
	return (0);
}

int	fork_pipeline_child(t_pipeline_context *pipeline_ctx, int i)
{
	t_pipe_child_context	ctx;

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
		execute_pipe_child_with_redirections(pipeline_ctx->commands[i],
			&ctx, pipeline_ctx->shell);
		exit(127);
	}
	return (0);
}
