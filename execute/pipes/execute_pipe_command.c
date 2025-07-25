/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe_command.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 22:24:01 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 14:39:26 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	execute_pipe_command(t_split cmd, t_pipe_context *ctx, t_shell *shell)
{
	char	**args;
	pid_t	pid;

	if (cmd.size == 0)
		return (1);
	args = split_to_args(cmd);
	if (!args)
		return (1);
	expand_command_args(args, shell);
	if (!args[0])
	{
		free_args(args);
		return (0);
	}
	if (is_builtin(args[0]))
		pid = handle_builtin_command(args, ctx, shell);
	else
		pid = handle_external_command(args, ctx, shell);
	free_args(args);
	return (pid);
}

void	setup_pipe_redirection(int cmd_index, int cmd_count, int **pipes)
{
	if (cmd_index > 0)
		dup2(pipes[cmd_index - 1][0], STDIN_FILENO);
	if (cmd_index < cmd_count - 1)
		dup2(pipes[cmd_index][1], STDOUT_FILENO);
}
