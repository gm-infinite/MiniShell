/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_cleanup.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 15:36:38 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

void	free_child_memory(char **args, t_shell *shell)
{
	if (args)
		free_args(args);
	if (shell)
	{
		free_env(shell);
	}
}

void	free_heredoc_child_memory(t_shell *shell, t_heredoc_params *params)
{
	if (params->args)
		free_args(params->args);
	if (params->clean_args)
		free_args(params->clean_args);
	if (params->delimiter)
		free(params->delimiter);
	if (shell)
	{
		free_env(shell);
		free_split(&shell->split_input);
		free(shell->current_input);
	}
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
