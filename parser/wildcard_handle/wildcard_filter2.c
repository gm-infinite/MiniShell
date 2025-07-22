/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_filter2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 13:57:09 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/19 09:26:40 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wildcard_handler.h"

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

static char	*process_wildcard_pattern(const char *wildcard)
{
	char	*result;
	int		i;
	int		j;
	int		in_single_quotes;
	int		in_double_quotes;

	result = malloc(ft_strlen(wildcard) + 1);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	in_single_quotes = 0;
	in_double_quotes = 0;
	while (wildcard[i])
	{
		if (wildcard[i] == '\'' && !in_double_quotes)
		{
			in_single_quotes = !in_single_quotes;
			i++;
			continue ;
		}
		if (wildcard[i] == '"' && !in_single_quotes)
		{
			in_double_quotes = !in_double_quotes;
			i++;
			continue ;
		}
		if (wildcard[i] == '*' && (in_single_quotes || in_double_quotes))
			result[j++] = '\001';
		else
			result[j++] = wildcard[i];
		i++;
	}
	result[j] = '\0';
	return (result);
}

static t_split	create_filter(const char *wildcard)
{
	t_split		filter;
	char		*processed_wildcard;
	char		*temp;
	int			i;
	int			j;

	temp = process_wildcard_pattern(wildcard);
	if (!temp)
		return (create_split(NULL, 0));
	processed_wildcard = temp;
	filter.size = calculate_filter_size(processed_wildcard);
	filter.start = (char **)ft_calloc(filter.size + 1, sizeof(char *));
	if (filter.start == NULL)
	{
		free(processed_wildcard);
		return (create_split(NULL, 0));
	}
	extract_wildcard_parts(processed_wildcard, &filter);
	i = -1;
	while (++i < filter.size && filter.start[i])
	{
		j = -1;
		while (filter.start[i][++j])
			if (filter.start[i][j] == '\001')
				filter.start[i][j] = '*';
	}
	free(processed_wildcard);
	return (filter);
}

void	apply_filter(t_split cur_dir, char *check_list, char *wildcard,
		t_shell *shell)
{
	t_split	filter;
	char	*processed_wildcard;

	processed_wildcard = process_wildcard_pattern(wildcard);
	if (!processed_wildcard)
		return ;
	filter = create_filter(wildcard, shell);
	apply_filter_minlen(filter, cur_dir, check_list, processed_wildcard);
	apply_filter_start(filter, cur_dir, check_list);
	apply_filter_end(filter, cur_dir, check_list);
	apply_filter_middle(filter, cur_dir, check_list);
	free(processed_wildcard);
	free_split(&filter);
}
