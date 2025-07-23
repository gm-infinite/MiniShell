/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:50:25 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/23 17:02:14 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	execute_pipeline_with_redirections(t_split split, t_shell *shell)
{
	t_split	*commands;
	int		cmd_count;
	int		exit_status;

	commands = split_by_pipes(split, &cmd_count);
	if (!commands)
		return (1);
	if (cmd_count == 1)
	{
		exit_status = execute_with_redirections(commands[0], shell);
		free(commands);
		return (exit_status);
	}
	return (setup_and_execute_pipeline(commands, cmd_count, shell));
}

void	execute_pipe_child_with_redirections(t_split cmd,
		t_pipe_child_context *ctx, t_shell *shell)
{
	char					**args;
	t_redir_fds				fds;
	t_pipe_setup_context	pipe_ctx;
	int						fd_values[3];

	fd_values[0] = STDIN_FILENO;
	fd_values[1] = STDOUT_FILENO;
	fd_values[2] = STDERR_FILENO;
	fds.input_fd = &fd_values[0];
	fds.output_fd = &fd_values[1];
	fds.stderr_fd = &fd_values[2];
	pipe_ctx.cmd_index = ctx->cmd_index;
	pipe_ctx.cmd_count = ctx->cmd_count;
	pipe_ctx.pipes = ctx->pipes;
	pipe_ctx.input_fd = &fd_values[0];
	pipe_ctx.output_fd = &fd_values[1];
	setup_pipe_fds(&pipe_ctx);
	args = parse_redirections(cmd, &fds, shell);
	if (!args)
		exit(1);
	redirect_fds(fd_values[0], fd_values[1], fd_values[2]);
	close_all_pipes(ctx->pipes, ctx->cmd_count);
	setup_child_signals();
	process_and_check_args(args, shell);
	execute_child_command(args, shell);
}
