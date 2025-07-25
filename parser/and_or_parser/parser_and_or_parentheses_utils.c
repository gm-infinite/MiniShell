/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_and_or_parentheses_utils.c                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:15:24 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/21 01:00:18 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	is_singlepar(char *start, char *end)
{
	if (start == end && (size_t)(countchr_str(start, '(')
		+ countchr_str(start, ')')) == ft_strlen(start))
		return (1);
	return (0);
}

void	remove_outer_par(char *start, char *end)
{
	int	start_len;

	if (start[0] == '(' && end[ft_strlen(end) - 1] == ')')
	{
		end[ft_strlen(end) - 1] = '\0';
		start_len = ft_strlen(start);
		ft_memmove(&(start[0]), &(start[1]), start_len - 1);
		start[start_len - 1] = '\0';
	}
}

void	clean_empties(t_split *split)
{
	char	*start;
	char	*end;

	start = split->start[0];
	end = split->start[split->size - 1];
	if (split->size > 1 && start[0] == '\0')
	{
		split->start = &(split->start[1]);
		split->size--;
	}
	if (end[0] == '\0')
		split->size--;
}
