/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_filter2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 13:57:09 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/06/01 14:09:34 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wildcard_handler.h"

static t_split	create_filter(const char *wildcard)
{
	t_split		filter;
	int			indx[3];
	char 		quote;

	quote  =  0;
	ft_memset(indx, 0, sizeof(int) * 3);
	filter.size = countchr_not_quote((char *)wildcard, '*') + 1;
	filter.start = (char **)ft_calloc(filter.size + 1, sizeof(char *));
	if (filter.start == NULL)
		return (create_split(NULL, 0));
	while (indx[0] <= ft_strlen(wildcard))
	{
		while (wildcard[indx[1]] != '\0' && wildcard[indx[1]] != '*')
			indx[1]++;
		if (indx[0] != indx[1])
			filter.start[indx[2]++] = ft_substr(wildcard, indx[0], indx[1] - indx[0]);
		else
			filter.start[indx[2]++] = ft_strdup("");
		indx[0] = indx[1] + 1;
		indx[1] = indx[0];
	}
	return (filter);
}

void apply_filter(t_split cur_dir, char *check_list, char *wildcard)
{
	t_split filter;

	filter = create_filter(wildcard);
	apply_filter_minlen(filter, cur_dir, check_list, wildcard);
	apply_filter_start(filter, cur_dir, check_list);
	apply_filter_end(filter, cur_dir, check_list);
	apply_filter_middle(filter, cur_dir, check_list);
	free_split(&filter);
}