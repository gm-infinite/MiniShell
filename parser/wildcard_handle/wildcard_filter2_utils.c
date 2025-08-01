/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_filter2_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 10:59:37 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/25 12:26:54 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wildcard_handler.h"

static int	calculate_filter_size(char *processed_wildcard)
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

int	setup_filter_memory(t_split *filter, char *processed_wildcard)
{
	filter->size = calculate_filter_size(processed_wildcard);
	filter->start = (char **)ft_calloc(filter->size + 1, sizeof(char *));
	if (filter->start == NULL)
		return (0);
	return (1);
}
