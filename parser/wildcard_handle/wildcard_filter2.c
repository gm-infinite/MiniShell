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

static t_split	create_filter(const char *wildcard, t_shell *shell)
{
	t_split		filter;
	int			indx[3];
	char		*processed_wildcard;
	char		*temp_substr;

	ft_memset(indx, 0, sizeof(int) * 3);
	processed_wildcard = process_quotes((char *)wildcard, shell);
	if (!processed_wildcard)
		return (create_split(NULL, 0));
	
	filter.size = ft_strlen(processed_wildcard) == 0 ? 1 : 1;
	// Count asterisks in the processed wildcard to determine filter size
	for (int i = 0; processed_wildcard[i]; i++)
		if (processed_wildcard[i] == '*')
			filter.size++;
			
	filter.start = (char **)ft_calloc(filter.size + 1, sizeof(char *));
	if (filter.start == NULL)
	{
		free(processed_wildcard);
		return (create_split(NULL, 0));
	}
	while (indx[0] <= (int)ft_strlen(processed_wildcard))
	{
		while (processed_wildcard[indx[1]] != '\0' && processed_wildcard[indx[1]] != '*')
			indx[1]++;
		if (indx[0] != indx[1])
		{
			temp_substr = ft_substr(processed_wildcard, indx[0], indx[1] - indx[0]);
			filter.start[indx[2]] = temp_substr;
			indx[2]++;
		}
		else
		{
			filter.start[indx[2]] = ft_strdup("");
			indx[2]++;
		}
		indx[0] = indx[1] + 1;
		indx[1] = indx[0];
	}
	free(processed_wildcard);
	return (filter);
}

void	apply_filter(t_split cur_dir, char *check_list, char *wildcard, t_shell *shell)
{
	t_split	filter;
	char	*processed_wildcard;

	filter = create_filter(wildcard, shell);
	processed_wildcard = process_quotes(wildcard, shell);
	apply_filter_minlen(filter, cur_dir, check_list, processed_wildcard);
	apply_filter_start(filter, cur_dir, check_list);
	apply_filter_end(filter, cur_dir, check_list);
	apply_filter_middle(filter, cur_dir, check_list);
	free(processed_wildcard);
	free_split(&filter);
}
