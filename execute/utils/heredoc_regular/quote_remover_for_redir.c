/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_remover_for_redir.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 16:16:51 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 05:09:54 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../main/minishell.h"

char	*remove_quotes_for_redirection(char *str)
{
	char			*result;
	int				i;
	int				j;
	unsigned char	quote_state;

	if (!str)
		return (NULL);
	result = malloc(ft_strlen(str) + 1);
	if (!result)
		return (NULL);
	i = -1;
	j = -1;
	quote_state = 0;
	while (str[++i])
	{
		if (str[i] == '\'' && !(quote_state & 2))
			quote_state ^= 1;
		else if (str[i] == '"' && !(quote_state & 1))
			quote_state ^= 2;
		else
			result[++j] = str[i];
	}
	result[++j] = '\0';
	return (result);
}

char	*process_heredoc_delimiter(char *filename)
{
	char	*result;

	result = remove_quotes_for_redirection(filename);
	if (!result)
	{
		result = ft_strdup(filename);
		if (!result)
			return (NULL);
	}
	return (result);
}
