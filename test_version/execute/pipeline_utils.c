/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 20:17:42 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	wait_for_children(pid_t *pids, int cmd_count)
{
	int	status;
	int	exit_status;
	int	i;

	exit_status = 0;
	i = 0;
	while (i < cmd_count)
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
		i++;
	}
	return (exit_status);
}

int	setup_pipeline_resources(t_split **commands, int ***pipes, pid_t **pids,
							int cmd_count)
{
	if (!create_pipes_array(pipes, cmd_count))
	{
		free(*commands);
		return (0);
	}
	*pids = malloc(sizeof(pid_t) * cmd_count);
	if (!*pids)
	{
		cleanup_pipes(*pipes, cmd_count);
		free(*commands);
		return (0);
	}
	return (1);
}

int	execute_pipeline_children(t_pipeline_context *pipeline_ctx)
{
	int						i;
	t_pipe_child_context	ctx;

	i = 0;
	while (i < pipeline_ctx->cmd_count)
	{
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
		i++;
	}
	close_all_pipes(pipeline_ctx->pipes, pipeline_ctx->cmd_count);
	return (wait_for_children(pipeline_ctx->pids, pipeline_ctx->cmd_count));
}

void	cleanup_pipeline_resources(t_split *commands, int **pipes, pid_t *pids,
								int cmd_count)
{
	cleanup_pipes(pipes, cmd_count);
	free(pids);
	free(commands);
}
