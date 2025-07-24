/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 14:26:56 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include "../main/get_next_line.h"

static int	wait_for_children(pid_t *pids, int cmd_count)
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

static void	cleanup_pipes_safe(int **pipes, int cmd_count)
{
	int	i;

	if (!pipes)
		return ;
	i = 0;
	while (i < cmd_count - 1 && pipes[i])
	{
		if (pipes[i][0] >= 0)
			close(pipes[i][0]);
		if (pipes[i][1] >= 0)
			close(pipes[i][1]);
		free(pipes[i]);
		pipes[i] = NULL;
		i++;
	}
	free(pipes);
}

int	setup_pipeline_resources(t_split **commands, int ***pipes, pid_t **pids,
		int cmd_count)
{
	if (!create_pipes_array(pipes, cmd_count))
	{
		free(*commands);
		*commands = NULL;
		return (0);
	}
	*pids = malloc(sizeof(pid_t) * cmd_count);
	if (!*pids)
	{
		cleanup_pipes_safe(*pipes, cmd_count);
		*pipes = NULL;
		free(*commands);
		*commands = NULL;
		return (0);
	}
	return (1);
}

int	execute_pipeline_children(t_pipeline_context *pipeline_ctx)
{
	int	i;

	if (preprocess_heredocs_in_pipeline(pipeline_ctx) != 0)
		return (1);
	i = -1;
	while (++i < pipeline_ctx->cmd_count)
	{
		if (fork_pipeline_child(pipeline_ctx, i) != 0)
			return (1);
	}
	close_all_pipes(pipeline_ctx->pipes, pipeline_ctx->cmd_count);
	return (wait_for_children(pipeline_ctx->pids, pipeline_ctx->cmd_count));
}

void	cleanup_pipeline_resources(t_split *commands, int **pipes, pid_t *pids,
		int cmd_count)
{
	if (pipes)
		cleanup_pipes_safe(pipes, cmd_count);
	if (pids)
		free(pids);
	if (commands)
		free(commands);
}
