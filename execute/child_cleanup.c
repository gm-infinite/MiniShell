/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_cleanup.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/23 19:11:29 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

void	free_child_memory(char **args, t_shell *shell)
{
	if (args)
		free_args(args);
	if (shell)
	{
		free_environment(shell);
		
	}
}

void	free_child_pipeline_memory(char **args, t_shell *shell, 
	t_pipeline_cleanup *cleanup)
{
	int	i;

	if (args)
		free_args(args);
	if (shell)
	{
		free_split(&shell->split_input);
		free_environment(shell);
		free(shell->current_input);
	}
	if (cleanup)
	{
		if (cleanup->commands)
			free(cleanup->commands);
		if (cleanup->pipes)
		{
			i = 0;
			while (i < cleanup->cmd_count - 1)
			{
				if (cleanup->pipes[i])
					free(cleanup->pipes[i]);
				i++;
			}
			free(cleanup->pipes);
		}
		if (cleanup->pids)
			free(cleanup->pids);
	}
}
