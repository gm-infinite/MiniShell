/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_and_or_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 00:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/21 00:59:35 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static char	*allocate_cut_indexes(t_split split)
{
	return (ft_calloc(count_str_split(split, "||", 1)
			+ count_str_split(split, "&&", 1) + 1, sizeof(char)));
}

static int	process_cut_index(t_split split, int i, int par)
{
	if (par == 0)
	{
		if (ft_strncmp("||", split.start[i], 3) == 0)
			return (1);
		else if (ft_strncmp("&&", split.start[i], 3) == 0)
			return (0);
	}
	return (-1);
}

char	*get_cut_indexs(t_split split)
{
	int		i;
	int		par;
	char	*ret;
	int		check;
	int		result;

	i = -1;
	par = 0;
	check = 0;
	ret = allocate_cut_indexes(split);
	if (ret == NULL)
		return (NULL);
	while (++i < split.size)
	{
		par += countchr_not_quote(split.start[i], '(');
		result = process_cut_index(split, i, par);
		if (result != -1)
			ret[check++] = '0' + result;
		par -= countchr_not_quote(split.start[i], ')');
	}
	return (ret);
}

static int	count_parentheses_in_string(char *str)
{
	int	j;
	int	par;
	int	quote_state;

	j = -1;
	par = 0;
	quote_state = 0;
	while (str[++j] != '\0')
	{
		if (str[j] == '\'' && !(quote_state & 2))
			quote_state ^= 1;
		else if (str[j] == '"' && !(quote_state & 1))
			quote_state ^= 2;
		else if (!quote_state)
			par += (str[j] == '(') - (str[j] == ')');
	}
	return (par);
}

int	paranthesis_parity_check(t_split split)
{
	int	i;
	int	par;

	i = -1;
	par = 0;
	while (++i < split.size)
		par += count_parentheses_in_string(split.start[i]);
	if (par == 0)
		return (1);
	return (0);
}
