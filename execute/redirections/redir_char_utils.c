/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_char_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 20:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 16:52:01 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

char	*handle_quote_char(char *result, char quote_char)
{
	char	quote_str[2];
	char	*temp;

	quote_str[0] = quote_char;
	quote_str[1] = '\0';
	temp = ft_strjoin(result, quote_str);
	free(result);
	return (temp);
}

char	*handle_double_quote_case(char *result, int *i,
		int *in_double_quotes, int in_single_quotes)
{
	if (!in_single_quotes)
	{
		result = handle_quote_char(result, '"');
		*in_double_quotes = !*in_double_quotes;
		(*i)++;
		return (result);
	}
	return (NULL);
}
