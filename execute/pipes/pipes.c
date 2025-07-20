/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 15:24:56 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:23:24 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static int	handle_single_command(t_split *commands, t_shell *shell)
{
	char	**args;
	int		exit_status;

	args = split_to_args(commands[0]);
	if (!args)
	{
		free(commands);
		return (1);
	}
	exit_status = execute_single_command(args, shell);
	free_args(args);
	free(commands);
	return (exit_status);
}

static int	create_pipeline_processes(t_split *commands, int **pipes,
							pid_t *pids, int cmd_count, t_shell *shell)
{
	int	i;
	int	exit_status;

	exit_status = 0;
	i = 0;
	while (i < cmd_count)
	{
		commands[i] = process_parentheses_in_split(commands[i], shell);
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			exit_status = 1;
			break ;
		}
		else if (pids[i] == 0)
		{
			execute_pipe_child(commands[i], i, pipes, cmd_count, shell);
			exit(127);
		}
		i++;
	}
	return (exit_status);
}

static int	allocate_pipeline_resources(t_split *commands, int ***pipes,
							pid_t **pids, int cmd_count)
{
	if (!create_pipeline_pipes(pipes, cmd_count))
	{
		free(commands);
		return (0);
	}
	*pids = malloc(sizeof(pid_t) * cmd_count);
	if (!*pids)
	{
		cleanup_pipeline_pipes(*pipes, cmd_count);
		free(commands);
		return (0);
	}
	return (1);
}

static void	setup_pipeline_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

int	execute_pipeline(t_split split, t_shell *shell)
{
	t_split	*commands;
	int		cmd_count;
	int		**pipes;
	pid_t	*pids;
	int		exit_status;

	split = process_parentheses_in_split(split, shell);
	commands = split_by_pipes(split, &cmd_count);
	if (!commands)
		return (1);
	if (cmd_count == 1)
		return (handle_single_command(commands, shell));
	if (!allocate_pipeline_resources(commands, &pipes, &pids, cmd_count))
		return (1);
	setup_pipeline_signals();
	exit_status = create_pipeline_processes(commands, pipes, pids, cmd_count,
			shell);
	cleanup_pipeline_pipes(pipes, cmd_count);
	if (exit_status == 0)
		exit_status = wait_for_pipeline_processes(pids, cmd_count);
	setup_signals();
	free(pids);
	free(commands);
	return (exit_status);
}
