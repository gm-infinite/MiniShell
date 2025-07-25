/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes_processing_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 00:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 19:57:43 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	handle_escaped_quote(char *str, char *result, int i, int j)
{
	if (str[i] == '\001' && str[i + 1] == '\'')
	{
		result[j] = '\'';
		return (1);
	}
	return (0);
}

void	update_quote_states(char c, int *states)
{
	if (c == '\'' && !states[1])
		states[0] = !states[0];
	else if (c == '"' && !states[0])
		states[1] = !states[1];
}

int	should_copy_character(char c, int *states)
{
	if (c == '\'' && !states[1])
		return (0);
	if (c == '"' && !states[0])
		return (0);
	return (1);
}

int	process_quote_character(char *str, char *result, int *indices, int *states)
{
	int	i;
	int	j;

	i = indices[0];
	j = indices[1];
	if (handle_escaped_quote(str, result, i, j))
	{
		indices[1] = j + 1;
		indices[0] = i + 2;
		return (1);
	}
	if (should_copy_character(str[i], states))
	{
		result[j] = str[i];
		indices[1] = j + 1;
	}
	update_quote_states(str[i], states);
	indices[0] = i + 1;
	return (0);
}
