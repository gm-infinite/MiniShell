/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_by_pipes.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:11:20 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 13:28:27 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static t_split	*allocate_single_command(t_split split)
{
	t_split	*commands;

	commands = malloc(sizeof(t_split));
	if (!commands)
		return (NULL);
	commands[0] = split;
	return (commands);
}

static void	split_commands_by_pipes(t_split split, t_split *commands)
{
	int	i;
	int	cmd_idx;
	int	start;

	cmd_idx = 0;
	start = 0;
	i = 0;
	while (i <= split.size)
	{
		if (i == split.size || (ft_strncmp(split.start[i], "|", 2) == 0))
		{
			commands[cmd_idx].start = &split.start[start];
			commands[cmd_idx].size = i - start;
			cmd_idx++;
			start = i + 1;
		}
		i++;
	}
}

int	count_pipes(t_split split)
{
	int	i;
	int	pipe_count;

	pipe_count = 0;
	i = 0;
	while (i < split.size)
	{
		if (ft_strncmp(split.start[i], "|", 2) == 0)
			pipe_count++;
		i++;
	}
	return (pipe_count);
}

t_split	*split_by_pipes(t_split split, int *cmd_count)
{
	t_split	*commands;
	int		pipe_count;

	pipe_count = count_pipes(split);
	*cmd_count = pipe_count + 1;
	if (pipe_count == 0)
		return (allocate_single_command(split));
	commands = malloc(sizeof(t_split) * (*cmd_count));
	if (!commands)
		return (NULL);
	split_commands_by_pipes(split, commands);
	return (commands);
}
