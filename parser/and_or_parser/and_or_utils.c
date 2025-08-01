/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   and_or_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:05:57 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/25 11:06:06 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static int	check_symbol(t_split split, char *find, int flag)
{
	int	count;
	int	i;

	i = 0;
	count = 0;
	while (i < split.size)
	{
		count = count + countchr_not_quote(split.start[i], '(');
		if (flag == 0 || count == 0)
		{
			if (ft_strncmp(split.start[i], find, ft_strlen(find)) == 0)
				return (i);
		}
		count = count - countchr_not_quote(split.start[i], ')');
		i++;
	}
	return (-2);
}

int	countchr_str(char *str, char c)
{
	int	j;
	int	count;

	j = 0;
	count = 0;
	if (str == NULL || *str == '\0')
		return (0);
	while (str[j] != '\0')
	{
		if (str[j] == c)
			count++;
		j++;
	}
	return (count);
}

static int	countchr_quote(char *str, char c)
{
	int	j;
	int	inquote;
	int	count;

	j = 0;
	count = 0;
	inquote = 0;
	if (str == NULL || *str == '\0')
		return (0);
	while (str[j] != '\0')
	{
		inquote ^= (str[j] == '\"' && inquote != 2);
		inquote ^= 2 * (str[j] == '\'' && inquote != 1);
		if (inquote != 0 && str[j] == c)
			count++;
		j++;
	}
	return (count);
}

int	countchr_not_quote(char *str, char c)
{
	return (countchr_str(str, c) - countchr_quote(str, c));
}

int	check_single_par(t_split split)
{
	char	*start;
	char	*end;

	start = split.start[0];
	end = split.start[split.size - 1];
	if (start[0] == '(' && end[ft_strlen(end) - 1] == ')'
		&& check_symbol(split, "||", 1) < 0
		&& check_symbol(split, "&&", 1) < 0)
		return (1);
	return (0);
}
