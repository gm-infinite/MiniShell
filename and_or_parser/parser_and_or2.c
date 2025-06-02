/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_and_or2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:15:24 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/06/02 15:09:28 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

// counts exact str in split's str array . if flag = 1 doesn't paranthesis
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
		if ((flag == 0 || par == 0) && (strncmp(str, split.start[i], 4) == 0))
			count++;
		par -= countchr_not_quote(split.start[i], ')');
		i++;
	}
	return (count);
}
