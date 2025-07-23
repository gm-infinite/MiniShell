/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_cleanup.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/23 17:56:29 by kuzyilma         ###   ########.fr       */
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
