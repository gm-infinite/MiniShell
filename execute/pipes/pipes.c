/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 15:24:56 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 14:30:48 by kuzyilma         ###   ########.fr       */
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

static int	create_pipeline_processes(t_process_pipeline_context *proc_ctx)
{
	int	i;
	int	exit_status;

	exit_status = 0;
	i = 0;
	while (i < proc_ctx->cmd_count)
	{
		proc_ctx->pids[i] = fork();
		if (proc_ctx->pids[i] == -1)
		{
			perror("fork");
			exit_status = 1;
			break ;
		}
		else if (proc_ctx->pids[i] == 0)
			setup_and_execute_child(proc_ctx, i);
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

static int	execute_multi_command_pipeline(t_split *commands, int cmd_count,
		t_shell *shell)
{
	t_process_pipeline_context	proc_ctx;
	int							exit_status;

	if (!allocate_pipeline_resources(commands, &proc_ctx.pipes,
			&proc_ctx.pids, cmd_count))
		return (1);
	setup_pipeline_signals();
	proc_ctx.commands = commands;
	proc_ctx.cmd_count = cmd_count;
	proc_ctx.shell = shell;
	exit_status = create_pipeline_processes(&proc_ctx);
	cleanup_pipeline_pipes(proc_ctx.pipes, cmd_count);
	if (exit_status == 0)
		exit_status = wait_for_pipeline_processes(proc_ctx.pids, cmd_count);
	setup_signals();
	free(proc_ctx.pids);
	free(commands);
	return (exit_status);
}

int	execute_pipeline(t_split split, t_shell *shell)
{
	t_split	*commands;
	int		cmd_count;
	int		result;

	commands = split_by_pipes(split, &cmd_count);
	if (!commands)
		return (1);
	if (cmd_count == 1)
		result = handle_single_command(commands, shell);
	else
		result = execute_multi_command_pipeline(commands, cmd_count, shell);
	return (result);
}
