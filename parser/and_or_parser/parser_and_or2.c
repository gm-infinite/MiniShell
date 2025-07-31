/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_and_or2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:07:53 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/25 11:08:01 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	count_str_split(t_split split, const char *str, int flag)
{
	int	i;
	int	count;
	int	par;

	i = 0;
	count = 0;
	par = 0;
	while (i < split.size)
	{
		par += countchr_not_quote(split.start[i], '(');
		if ((flag == 0 || par == 0)
			&& (ft_strncmp(str, split.start[i], 4) == 0))
			count++;
		par -= countchr_not_quote(split.start[i], ')');
		i++;
	}
	return (count);
}
