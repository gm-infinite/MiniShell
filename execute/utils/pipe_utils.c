/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 11:05:09 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 20:04:00 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

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

static int	create_pipes_array(int ***pipes, int cmd_count)
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

int	setup_pipe(t_split **commands, int ***pipes, pid_t **pids,
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

void	clean_pipe(t_split *commands, int **pipes, pid_t *pids,
		int cmd_count)
{
	if (pipes)
		cleanup_pipes_safe(pipes, cmd_count);
	if (pids)
		free(pids);
	if (commands)
		free(commands);
}
