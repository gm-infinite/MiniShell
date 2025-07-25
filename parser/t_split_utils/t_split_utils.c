/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   t_split_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:53:30 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/03/25 16:13:24 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "t_split.h"

int	strnsplit(t_split split, char *find)
{
	int	i;

	if (find == NULL || *find == '\0')
		return (-2);
	i = 0;
	while (i < split.size)
	{
		if (ft_strncmp(split.start[i], find, ft_strlen(find)) == 0)
			return (i);
		i++;
	}
	return (-1);
}

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

void	rearrange_split(t_split *split)
{
	int	i;
	int	j;

	i = 0;
	while (split->start[i] != NULL)
		i++;
	j = i + 1;
	while (j < split->size)
	{
		if (split->start[j] != NULL)
		{
			split->start[i] = split->start[j];
			split->start[j] = NULL;
			i++;
		}
		j++;
	}
	split->size = i;
}
