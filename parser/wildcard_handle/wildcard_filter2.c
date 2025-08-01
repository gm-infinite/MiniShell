/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_filter2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:00:02 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/25 12:16:09 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wildcard_handler.h"

static void	restore_asterisks_in_filter(t_split *filter)
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

static void	extract_wildcard_parts(char *processed_wildcard, t_split *filter)
{
	int		indx[3];
	char	*temp_substr;

	ft_memset(indx, 0, sizeof(int) * 3);
	while (indx[0] <= (int)ft_strlen(processed_wildcard))
	{
		while (processed_wildcard[indx[1]] != '\0'
			&& processed_wildcard[indx[1]] != '*')
			indx[1]++;
		if (indx[0] != indx[1])
		{
			temp_substr = ft_substr(processed_wildcard, indx[0],
					indx[1] - indx[0]);
			filter->start[indx[2]] = temp_substr;
			indx[2]++;
		}
		else
		{
			filter->start[indx[2]] = ft_strdup("");
			indx[2]++;
		}
		indx[0] = indx[1] + 1;
		indx[1] = indx[0];
	}
}

static char	*process_wildcard_pattern(const char *wildcard)
{
	char	*result;
	int		vars[4];

	result = init_pattern_result(wildcard, vars);
	if (!result)
		return (NULL);
	process_wildcard_loop(wildcard, result, vars);
	result[vars[1]] = '\0';
	return (result);
}

static t_split	create_filter(const char *wildcard)
{
	t_split		filter;
	char		*processed_wildcard;
	char		*temp;

	temp = process_wildcard_pattern(wildcard);
	if (!temp)
		return (create_split(NULL, 0));
	processed_wildcard = temp;
	if (!setup_filter_memory(&filter, processed_wildcard))
	{
		free(processed_wildcard);
		return (create_split(NULL, 0));
	}
	extract_wildcard_parts(processed_wildcard, &filter);
	restore_asterisks_in_filter(&filter);
	free(processed_wildcard);
	return (filter);
}

void	apply_filter(t_split cur_dir, char *check_list, char *wildcard)
{
	t_split	filter;
	char	*processed_wildcard;

	processed_wildcard = process_wildcard_pattern(wildcard);
	if (!processed_wildcard)
		return ;
	filter = create_filter(processed_wildcard);
	apply_filter_minlen(filter, cur_dir, check_list, processed_wildcard);
	apply_filter_start(filter, cur_dir, check_list);
	apply_filter_end(filter, cur_dir, check_list);
	apply_filter_middle(filter, cur_dir, check_list);
	free(processed_wildcard);
	free_split(&filter);
}
