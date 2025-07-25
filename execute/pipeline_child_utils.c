/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_child_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 21:53:59 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	process_command_redirections(char **args,
		t_pipe_ctx *pipeline_ctx, int i)
{
	int	j;

	j = 0;
	while (args[j])
	{
		if (is_redirection(args[j]) == 1)
		{
			if (!args[j + 1])
			{
				write(STDERR_FILENO, "syntax error: missing filename\n", 32);
				return (1);
			}
			if (heredoc_redir(args, j, pipeline_ctx, i) != 0)
				return (1);
			j += 2;
		}
		else
		{
			if (is_redirection(args[j]))
				j += 2;
			else
				j++;
		}
	}
	return (0);
}

int	heredoc_pipe(t_pipe_ctx *pipeline_ctx)
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

int	fork_pipe_child(t_pipe_ctx *pipeline_ctx, int i)
{
	t_child_ctx				ctx;
	t_child_redir_params	params;

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
		params.cmd = pipeline_ctx->commands[i];
		params.ctx = &ctx;
		params.shell = pipeline_ctx->shell;
		params.commands = pipeline_ctx->commands;
		params.pids = pipeline_ctx->pids;
		execute_child_redir(&params);
		exit(127);
	}
	return (0);
}
