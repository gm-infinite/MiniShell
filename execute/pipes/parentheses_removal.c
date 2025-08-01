/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parentheses_removal.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 10:40:17 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 10:51:52 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	first_par_removal(t_split *commands, int k,
	size_t len0, char *first)
{
	int	t;

	if (len0 == 1)
	{
		t = -1;
		while (++t + 1 < commands[k].size)
			commands[k].start[t] = commands[k].start[t + 1];
		commands[k].size--;
	}
	else
		ft_memmove(first, first + 1, len0);
}

void	parantheses_removal_helper(t_split *commands, int k)
{
	char	*first;
	char	*last;
	size_t	len0;
	size_t	lenn;

	if (commands[k].size > 0)
	{
		first = commands[k].start[0];
		len0 = ft_strlen(first);
		if (len0 > 0 && first[0] == '(')
			first_par_removal(commands, k, len0, first);
	}
	if (commands[k].size > 0)
	{
		last = commands[k].start[commands[k].size - 1];
		lenn = ft_strlen(last);
		if (lenn > 0 && last[lenn - 1] == ')')
		{
			if (lenn == 1)
				commands[k].size--;
			else
				last[lenn - 1] = '\0';
		}
	}
}
