/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_quote_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 13:57:09 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/19 09:26:40 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wildcard_handler.h"

int	handle_single_quote(const char *wildcard, int *i,
		int *in_single_quotes, int in_double_quotes)
{
	if (wildcard[*i] == '\'' && !in_double_quotes)
	{
		*in_single_quotes = !*in_single_quotes;
		(*i)++;
		return (1);
	}
	return (0);
}

int	handle_double_quote(const char *wildcard, int *i,
		int *in_double_quotes, int in_single_quotes)
{
	if (wildcard[*i] == '"' && !in_single_quotes)
	{
		*in_double_quotes = !*in_double_quotes;
		(*i)++;
		return (1);
	}
	return (0);
}

char	*init_pattern_result(const char *wildcard, int *vars)
{
	char	*result;

	result = malloc(ft_strlen(wildcard) + 1);
	if (!result)
		return (NULL);
	vars[0] = 0;
	vars[1] = 0;
	vars[2] = 0;
	vars[3] = 0;
	return (result);
}

void	process_wildcard_loop(const char *wildcard, char *result, int *vars)
{
	while (wildcard[vars[0]])
	{
		if (handle_single_quote(wildcard, &vars[0], &vars[2], vars[3]))
			continue ;
		if (handle_double_quote(wildcard, &vars[0], &vars[3], vars[2]))
			continue ;
		if (wildcard[vars[0]] == '*' && (vars[2] || vars[3]))
			result[vars[1]++] = '\001';
		else
			result[vars[1]++] = wildcard[vars[0]];
		vars[0]++;
	}
}
