/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe_command.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 22:24:01 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:31:52 by emgenc           ###   ########.fr       */
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
