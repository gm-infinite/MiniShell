/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_pipes.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 16:30:16 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/16 20:58:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

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
