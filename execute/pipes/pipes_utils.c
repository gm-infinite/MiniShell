/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:50:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 20:52:39 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	create_pipeline_pipes(int ***pipes, int cmd_count)
{
	int	i;

	*pipes = malloc(sizeof(int *) * (cmd_count - 1));
	if (!*pipes)
		return (0);
	i = 0;
	while (i < cmd_count - 1)
	{
		(*pipes)[i] = malloc(sizeof(int) * 2);
		if (pipe((*pipes)[i]) == -1)
		{
			perror("pipe");
			while (--i >= 0)
			{
				close((*pipes)[i][0]);
				close((*pipes)[i][1]);
				free((*pipes)[i]);
			}
			free(*pipes);
			return (0);
		}
		i++;
	}
	return (1);
}

void	cleanup_pipeline_pipes(int **pipes, int cmd_count)
{
	int	i;

	i = 0;
	while (i < cmd_count - 1)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		free(pipes[i]);
		i++;
	}
	free(pipes);
}

int	wait_for_pipeline_processes(pid_t *pids, int cmd_count)
{
	int	i;
	int	status;
	int	exit_status;

	exit_status = 0;
	i = 0;
	while (i < cmd_count)
	{
		if (pids[i] > 0)
		{
			waitpid(pids[i], &status, 0);
			if (i == cmd_count - 1)
			{
				if (WIFEXITED(status))
					exit_status = WEXITSTATUS(status);
				else if (WIFSIGNALED(status))
					exit_status = 128 + WTERMSIG(status);
			}
		}
		i++;
	}
	return (exit_status);
}

void	setup_child_redirection(t_pipe_context *ctx)
{
	int	i;

	if (ctx->input_fd != STDIN_FILENO)
	{
		dup2(ctx->input_fd, STDIN_FILENO);
		close(ctx->input_fd);
	}
	if (ctx->output_fd != STDOUT_FILENO)
	{
		dup2(ctx->output_fd, STDOUT_FILENO);
		close(ctx->output_fd);
	}
	i = 0;
	while (i < ctx->pipe_count)
	{
		close(ctx->all_pipes[i][0]);
		close(ctx->all_pipes[i][1]);
		i++;
	}
}
