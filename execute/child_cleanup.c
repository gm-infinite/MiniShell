/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_cleanup.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/23 17:26:17 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

/**
 * Free all memory allocated by a child process before exit/execve
 * This function should be called in child processes before calling exit()
 * or execve() to prevent memory leaks.
 * 
 * @param args Command arguments array to free
 * @param shell Shell structure containing environment and other allocated memory
 */
void	free_child_memory(char **args, t_shell *shell)
{
	if (args)
		free_args(args);
	if (shell)
	{
		free_environment(shell);
		
	}
}

/**
 * Free memory for child processes in pipelines, including the commands array
 * 
 * @param args Command arguments array to free
 * @param shell Shell structure containing environment and other allocated memory  
 * @param commands Pipeline commands array to free
 * @param pipes Pipeline pipes array to free
 * @param pids Pipeline process IDs array to free
 * @param cmd_count Number of commands in pipeline
 */
void	free_child_pipeline_memory(char **args, t_shell *shell, t_split *commands,
	int **pipes, pid_t *pids, int cmd_count)
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
	if (commands)
		free(commands);
	if (pipes)
	{
		i = 0;
		while (i < cmd_count - 1)
		{
			if (pipes[i])
				free(pipes[i]);
			i++;
		}
		free(pipes);
	}
	if (pids)
		free(pids);
}
