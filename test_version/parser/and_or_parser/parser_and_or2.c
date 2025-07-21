/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_and_or2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:15:24 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/16 20:58:00 by emgenc           ###   ########.fr       */
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
