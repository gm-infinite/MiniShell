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

int	execute_pipeline_children(t_split *commands, int **pipes, pid_t *pids,
							int cmd_count, t_shell *shell)
{
	int	i;

	i = 0;
	while (i < cmd_count)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			return (1);
		}
		else if (pids[i] == 0)
		{
			execute_pipe_child_with_redirections(commands[i], i, pipes,
				cmd_count, shell);
			exit(127);
		}
		i++;
	}
	close_all_pipes(pipes, cmd_count);
	return (wait_for_children(pids, cmd_count));
}

void	cleanup_pipeline_resources(t_split *commands, int **pipes, pid_t *pids,
								int cmd_count)
{
	cleanup_pipes(pipes, cmd_count);
	free(pids);
	free(commands);
}
