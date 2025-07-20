/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_filter.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 13:57:14 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/21 01:01:55 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wildcard_handler.h"

void	apply_filter_start(t_split filter, t_split cur_dir, char *check_list)
{
	int	i;

	i = 0;
	if (filter.start[0][0] == '\0')
	{
		while (i < cur_dir.size)
		{
			if (cur_dir.start[i][0] == '.')
				check_list[i] = '0';
			i++;
		}
	}
	else
	{
		while (i < cur_dir.size)
		{
			if (ft_strncmp(filter.start[0], cur_dir.start[i],
					ft_strlen(filter.start[0])))
				check_list[i] = '0';
			i++;
		}
	}
}

void	apply_filter_end(t_split filter, t_split cur_dir, char *check_list)
{
	int		i;
	char	*filter_end;

	filter_end = filter.start[filter.size - 1];
	i = 0;
	while (i < cur_dir.size && check_list[i] == '0')
		i++;
	if (filter.start[filter.size - 1][0] != '\0')
	{
		while (i < cur_dir.size)
		{
			if (ft_strrcmp(filter_end, cur_dir.start[i]))
				check_list[i] = '0';
			i++;
			while (check_list[i] == '0')
				i++;
		}
	}
}

void	apply_filter_minlen(t_split filter, t_split cur_dir,
							char *check_list, char *wildcard)
{
	int		i;
	int		minlen;

	i = 0;
	minlen = ft_strlen(wildcard) - filter.size + 1;
	while (i < cur_dir.size)
	{
		if ((int)ft_strlen(cur_dir.start[i]) < minlen)
			check_list[i] = '0';
		i++;
	}
}

static void	apply_filter_midsin(t_split filter, char *cur_dir_i,
								char *check_list_i)
{
	int		filter_i;
	char	*index;

	filter_i = 1;
	index = &(cur_dir_i[ft_strlen(filter.start[0])]);
	while (filter_i < filter.size - 1)
	{
		index = (ft_strnstr(index, filter.start[filter_i], ft_strlen(index)
					- ft_strlen(filter.start[filter.size - 1])));
		if (index == NULL)
		{
			*check_list_i = '0';
			return ;
		}
		if (ft_strlen(index) > ft_strlen(filter.start[filter_i]))
			index = &(index[ft_strlen(filter.start[filter_i])]);
		else
			index = &(index[ft_strlen(index)]);
		filter_i++;
	}
}

void	apply_filter_middle(t_split filter, t_split cur_dir, char *check_list)
{
	int		i;

	i = 0;
	if (filter.size < 3)
		return ;
	while (i < cur_dir.size)
	{
		if (check_list[i] != '0')
			apply_filter_midsin(filter, cur_dir.start[i], &(check_list[i]));
		i++;
	}
}
