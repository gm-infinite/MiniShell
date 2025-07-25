/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_handler.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 20:51:33 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/24 20:53:16 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	handle_builtin_command(char **args, t_pipe_context *ctx, t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		setup_child_signals();
		setup_child_redir(ctx);
		exit(execute_builtin(args, shell));
	}
	return (pid);
}

void	expand_command_args(char **args, t_shell *shell)
{
	char	*expanded;
	int		i;

	i = 0;
	while (args[i])
	{
		expanded = expandvar(args[i], shell);
		if (expanded && expanded != args[i])
		{
			free(args[i]);
			args[i] = expanded;
		}
		i++;
	}
}
