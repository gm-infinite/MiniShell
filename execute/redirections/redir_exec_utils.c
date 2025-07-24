/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_exec_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:20:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 21:47:39 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static int	save_and_redirect_builtin(int input_fd, int output_fd,
								int stderr_fd, int *saved_fds)
{
	saved_fds[0] = -1;
	saved_fds[1] = -1;
	saved_fds[2] = -1;
	if (input_fd != STDIN_FILENO)
	{
		saved_fds[0] = dup(STDIN_FILENO);
		dup2(input_fd, STDIN_FILENO);
		close(input_fd);
	}
	if (output_fd != STDOUT_FILENO)
	{
		saved_fds[1] = dup(STDOUT_FILENO);
		dup2(output_fd, STDOUT_FILENO);
		close(output_fd);
	}
	if (stderr_fd != STDERR_FILENO)
	{
		saved_fds[2] = dup(STDERR_FILENO);
		dup2(stderr_fd, STDERR_FILENO);
		close(stderr_fd);
	}
	return (0);
}

static void	restore_builtin_fds(int *saved_fds)
{
	if (saved_fds[0] != -1)
	{
		dup2(saved_fds[0], STDIN_FILENO);
		close(saved_fds[0]);
	}
	if (saved_fds[1] != -1)
	{
		dup2(saved_fds[1], STDOUT_FILENO);
		close(saved_fds[1]);
	}
	if (saved_fds[2] != -1)
	{
		dup2(saved_fds[2], STDERR_FILENO);
		close(saved_fds[2]);
	}
}

int	execute_builtin_with_redirect(redir_exec_ctx *ctx)
{
	int	saved_fds[3];
	int	exit_status;

	save_and_redirect_builtin(ctx->input_fd, ctx->output_fd,
		ctx->stderr_fd, saved_fds);
	exit_status = execute_builtin(ctx->args, ctx->shell);
	restore_builtin_fds(saved_fds);
	return (exit_status);
}
