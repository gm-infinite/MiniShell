/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_executor.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:20:33 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 10:42:54 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	setup_child(t_process_pipe_ctx *proc_ctx, int i)
{
	t_child_ctx		ctx;
	t_child_params	params;
	t_pipe_cleaner	cleanup;

	ctx.pipes = proc_ctx->pipes;
	ctx.cmd_index = i;
	ctx.cmd_count = proc_ctx->cmd_count;
	params.cmd = proc_ctx->commands[i];
	params.ctx = &ctx;
	params.shell = proc_ctx->shell;
	params.commands = proc_ctx->commands;
	params.pids = proc_ctx->pids;
	execute_pipe_child(&params);
	cleanup.commands = proc_ctx->commands;
	cleanup.pipes = proc_ctx->pipes;
	cleanup.pids = proc_ctx->pids;
	cleanup.cmd_count = proc_ctx->cmd_count;
	free_child_pipeline_memory(NULL, proc_ctx->shell, &cleanup);
	exit(127);
}

static int	allocate_pipeline_resources(t_split *commands, int ***pipes,
							pid_t **pids, int cmd_count)
{
	if (!create_pipes(pipes, cmd_count))
	{
		free(commands);
		return (0);
	}
	*pids = malloc(sizeof(pid_t) * cmd_count);
	if (!*pids)
	{
		pipe_clean(*pipes, cmd_count);
		free(commands);
		return (0);
	}
	return (1);
}

static int	create_pipeline_processes(t_process_pipe_ctx *proc_ctx)
{
	int		i;
	int		exit_status;
	pid_t	pid;

	exit_status = 0;
	i = 0;
	while (i < proc_ctx->cmd_count)
	{
		pid = fork();
		proc_ctx->pids[i] = pid;
		if (pid == -1)
		{
			perror("fork");
			exit_status = 1;
			break ;
		}
		else if (pid == 0)
			setup_child(proc_ctx, i);
		i++;
	}
	return (exit_status);
}

static int	wait_for_children(pid_t *pids, int cmd_count)
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

int	execute_multi_command_pipeline(t_split *commands, int cmd_count,
	t_shell *shell)
{
	t_process_pipe_ctx	proc_ctx;
	int					exit_status;
	int					k;

	k = -1;
	while (++k < cmd_count)
		parantheses_removal_helper(commands, k);
	if (!allocate_pipeline_resources(commands, &proc_ctx.pipes,
			&proc_ctx.pids, cmd_count))
		return (1);
	setup_pipe_signals();
	proc_ctx.commands = commands;
	proc_ctx.cmd_count = cmd_count;
	proc_ctx.shell = shell;
	exit_status = create_pipeline_processes(&proc_ctx);
	pipe_clean(proc_ctx.pipes, cmd_count);
	if (exit_status == 0)
		exit_status = wait_for_children(proc_ctx.pids, cmd_count);
	setup_signals();
	free(proc_ctx.pids);
	free(commands);
	return (exit_status);
}
