/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 11:02:48 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 20:06:14 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

void	free_args(char **args)
{
	int	i;

	if (!args)
		return ;
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

void	free_env(t_shell *shell)
{
	int	i;

	if (!shell->envp)
		return ;
	i = 0;
	while (shell->envp[i])
	{
		free(shell->envp[i]);
		i++;
	}
	free(shell->envp);
	shell->envp = NULL;
}

void	free_child_pipeline_memory(char **args, t_shell *shell,
	t_pipe_cleaner *cleanup)
{
	int	i;

	if (args)
		free_args(args);
	if (shell)
	{
		free_split(&shell->split_input);
		free_env(shell);
		free(shell->current_input);
	}
	if (cleanup)
	{
		if (cleanup->commands)
			free(cleanup->commands);
		if (cleanup->pipes)
		{
			i = -1;
			while (++i < cleanup->cmd_count - 1)
				if (cleanup->pipes[i])
					free(cleanup->pipes[i]);
			free(cleanup->pipes);
		}
		if (cleanup->pids)
			free(cleanup->pids);
	}
}
