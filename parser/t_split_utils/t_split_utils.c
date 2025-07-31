/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   t_split_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:03:41 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/25 12:28:45 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "t_split.h"

t_split	create_split(char **start, int size)
{
	t_split	ret;

	ret.start = start;
	ret.size = size;
	return (ret);
}

t_split	create_split_str(char *str)
{
	t_split	ret;
	int		i;

	ret.start = ft_split_quotes(str);
	if (ret.start == NULL)
	{
		ret.size = -1;
		return (ret);
	}
	i = 0;
	while (ret.start[i] != NULL)
		i++;
	ret.size = i;
	return (ret);
}

void	free_split(t_split *split)
{
	int	i;

	i = 0;
	while (i < split->size)
	{
		if (split->start[i] != NULL)
			free(split->start[i]);
		i++;
	}
	free(split->start);
	split->start = NULL;
	split->size = 0;
}
