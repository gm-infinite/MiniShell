/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 00:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 19:57:43 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static void	init_quote_removal(int *i, int *j, int *states)
{
	*i = 0;
	*j = 0;
	states[0] = 0;
	states[1] = 0;
}

char	*remove_quotes_from_string(char *str)
{
	char	*result;
	int		i;
	int		j;
	int		states[2];

	result = malloc(ft_strlen(str) * 2 + 1);
	if (!result)
		return (NULL);
	init_quote_removal(&i, &j, states);
	while (str[i])
	{
		if (str[i] == '\'' && !states[1])
			states[0] = !states[0];
		else if (str[i] == '"' && !states[0])
			states[1] = !states[1];
		else
		{
			result[j] = str[i];
			j++;
		}
		i++;
	}
	result[j] = '\0';
	return (result);
}

int	is_entirely_single_quoted(char *str)
{
	int	quote_count;
	int	k;

	if (str[0] != '\'' || str[ft_strlen(str) - 1] != '\'')
		return (0);
	quote_count = 0;
	k = 0;
	while (str[k])
	{
		if (str[k] == '\'')
			quote_count++;
		k++;
	}
	return (quote_count == 2);
}

int	is_in_single_quotes(char *str, int pos)
{
	int	i;
	int	in_single;
	int	in_double;

	i = 0;
	in_single = 0;
	in_double = 0;
	while (i < pos && str[i])
	{
		if (str[i] == '"' && !in_single)
			in_double = !in_double;
		else if (str[i] == '\'' && !in_double)
			in_single = !in_single;
		i++;
	}
	return (in_single);
}
