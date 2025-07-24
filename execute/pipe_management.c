/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_management.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 20:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 14:26:53 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	close_fds_and_pipes(int ***pipes, int *i)
{
	perror("pipe");
	free((*pipes)[*i]);
	while (--*i >= 0)
	{
		close((*pipes)[*i][0]);
		close((*pipes)[*i][1]);
		free((*pipes)[*i]);
	}
	free(*pipes);
	*pipes = NULL;
	return (0);
}

int	create_pipes_array(int ***pipes, int cmd_count)
{
	int	i;

	*pipes = malloc(sizeof(int *) * (cmd_count - 1));
	if (!*pipes)
		return (0);
	i = 0;
	while (i < cmd_count - 1)
	{
		(*pipes)[i] = malloc(sizeof(int) * 2);
		if (!(*pipes)[i])
		{
			while (--i >= 0)
				free((*pipes)[i]);
			free(*pipes);
			*pipes = NULL;
			return (0);
		}
		if (pipe((*pipes)[i]) == -1)
			return (close_fds_and_pipes(pipes, &i));
		i++;
	}
	return (1);
}

void	cleanup_pipes(int **pipes, int cmd_count)
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

void	close_all_pipes(int **pipes, int cmd_count)
{
	int	i;

	i = 0;
	while (i < cmd_count - 1)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

void	setup_pipe_fds(t_pipe_setup_context *ctx)
{
	if (ctx->cmd_index > 0)
		*(ctx->input_fd) = ctx->pipes[ctx->cmd_index - 1][0];
	if (ctx->cmd_index < ctx->cmd_count - 1)
		*(ctx->output_fd) = ctx->pipes[ctx->cmd_index][1];
}
