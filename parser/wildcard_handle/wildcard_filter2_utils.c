/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_filter2_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 13:57:09 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/19 09:26:40 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wildcard_handler.h"

void	restore_asterisks_in_filter(t_split *filter)
{
	int	i;
	int	j;

	i = -1;
	while (++i < filter->size && filter->start[i])
	{
		j = -1;
		while (filter->start[i][++j])
			if (filter->start[i][j] == '\001')
				filter->start[i][j] = '*';
	}
}

int	setup_filter_memory(t_split *filter, char *processed_wildcard)
{
	filter->size = calculate_filter_size(processed_wildcard);
	filter->start = (char **)ft_calloc(filter->size + 1, sizeof(char *));
	if (filter->start == NULL)
		return (0);
	return (1);
}

int	calculate_filter_size(char *processed_wildcard)
{
	int	size;
	int	i;

	if (ft_strlen(processed_wildcard) == 0)
		size = 1;
	else
		size = 1;
	i = 0;
	while (processed_wildcard[i])
	{
		if (processed_wildcard[i] == '*')
			size++;
		i++;
	}
	return (size);
}
