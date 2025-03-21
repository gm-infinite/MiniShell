/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   t_split_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:53:30 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/03/22 15:19:16 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "t_split.h"

// Error? Return -2. Not found? Return -1 Else return first occurrence position
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

// Creates a new t_split struct from the provided char** and size.
t_split	create_split(char **start, int size)
{
	t_split	ret;

	ret.start = start;
	ret.size = size;
	return (ret);
}

// Creates a t_split struct from a string using ft_split and counts elements.
t_split	create_split_str(char *str)
{
	t_split	ret;
	int		i;

	ret.start = ft_split(str, ' ');
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

// Frees the given t_split, NULLs the strings inside it, resets the size to 0
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

//returns a array of string from split with malloc. it isnt tested. I thought it would be useful
void rearrange_split(t_split *split)
{
	int i;
	int j;

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
