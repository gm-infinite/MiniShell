/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_by_pipes.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 16:31:39 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/16 20:58:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

t_split	*split_by_pipes(t_split split, int *cmd_count)
{
	t_split	*commands;
	int		pipe_count;
	int		i;
	int		cmd_idx;
	int		start;

	pipe_count = count_pipes(split);
	*cmd_count = pipe_count + 1;
	if (pipe_count == 0)
	{
		commands = malloc(sizeof(t_split));
		if (!commands)
			return (NULL);
		commands[0] = split;
		return (commands);
	}
	commands = malloc(sizeof(t_split) * (*cmd_count));
	if (!commands)
		return (NULL);
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
	return (commands);
}
