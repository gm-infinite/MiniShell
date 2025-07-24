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
	int		indices[2];
	int		states[2];

	result = malloc(ft_strlen(str) * 2 + 1);
	if (!result)
		return (NULL);
	init_quote_removal(&indices[0], &indices[1], states);
	while (str[indices[0]])
		if (process_quote_character(str, result, indices, states))
			continue ;
	result[indices[1]] = '\0';
	return (result);
}
