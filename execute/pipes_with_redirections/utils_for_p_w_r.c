/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_for_p_w_r.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 16:43:13 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 16:12:39 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

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

void	handle_empty_pipe_args(char **args)
{
	if (args[0] && args[0][0] == '\0')
		write(STDERR_FILENO, ": command not found\n", 20);
	free_args(args);
	if (args[0] && args[0][0] == '\0')
		exit(127);
	else
		exit(0);
}

void	setup_pipe_fds(t_pipe_setup_ctx *ctx)
{
	if (ctx->cmd_index > 0)
	{
		if (*(ctx->input_fd) == STDIN_FILENO)
			*(ctx->input_fd) = ctx->pipes[ctx->cmd_index - 1][0];
	}
	if (ctx->cmd_index < ctx->cmd_count - 1)
		*(ctx->output_fd) = ctx->pipes[ctx->cmd_index][1];
}

void	redirect_fds(int input_fd, int output_fd, int stderr_fd)
{
	if (input_fd != STDIN_FILENO)
	{
		dup2(input_fd, STDIN_FILENO);
		close(input_fd);
	}
	if (output_fd != STDOUT_FILENO)
	{
		dup2(output_fd, STDOUT_FILENO);
		close(output_fd);
	}
	if (stderr_fd != STDERR_FILENO)
	{
		dup2(stderr_fd, STDERR_FILENO);
		close(stderr_fd);
	}
}

void	process_and_check_args(char **args, t_shell *shell)
{
	int		i;
	char	*expanded;

	if (!args[0])
	{
		free_args(args);
		exit(0);
	}
	i = -1;
	while (args[++i])
	{
		expanded = expandvar_quoted(args[i], shell);
		if (expanded != args[i])
		{
			free(args[i]);
			args[i] = expanded;
		}
	}
	process_args_quotes(args, shell);
	if (!args[0])
		handle_empty_pipe_args(args);
}
