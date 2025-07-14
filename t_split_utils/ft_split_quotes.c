/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_quotes.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 16:30:24 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/13 16:32:57 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "t_split.h"

static int	count_tokens(const char *s, int *i, int *cnt)
{
	unsigned char	quote_state;

	while (s[*i])
	{
		while (s[*i] && (s[*i] == ' ' || s[*i] == '\t') && !quote_state)
			(*i)++;
		if (!s[*i])
			break ;
		(*cnt)++;
		quote_state = 0;
		while (s[*i])
		{
			if (s[*i] == '\'' && !(quote_state & 2))  // bit 1 (double quote) not set
				quote_state ^= 1;  // XOR toggle bit 0 (single quote)
			else if (s[*i] == '"' && !(quote_state & 1))  // bit 0 (single quote) not set
				quote_state ^= 2;  // XOR toggle bit 1 (double quote)
			else if ((s[*i] == ' ' || s[*i] == '\t') && !quote_state)
				break;  // End of token when no quotes active
			(*i)++;
		}
	}
	return (*cnt);
}

static void	extract_token(int *i, const char *s, char ***tab, int *j)
{
	int				start;
	unsigned char	quote_state;

	start = *i;
	quote_state = 0;
	
	// Process the entire token (including quoted sections)
	while (s[*i])
	{
		if (s[*i] == '\'' && !(quote_state & 2))  // bit 1 (double quote) not set
			quote_state ^= 1;  // XOR toggle bit 0 (single quote)
		else if (s[*i] == '"' && !(quote_state & 1))  // bit 0 (single quote) not set
			quote_state ^= 2;  // XOR toggle bit 1 (double quote)
		else if ((s[*i] == ' ' || s[*i] == '\t') && !quote_state)
			break;  // End of token when no quotes active
		(*i)++;
	}
	(*tab)[(*j)++] = ft_substr(s, start, (*i) - start);
}

char	**ft_split_quotes(const char *s)
{
	int		i;
	int		cnt;
	int		j;
	char	**tab;

	i = 0;
	cnt = 0;
	j = 0;
	tab = malloc(sizeof(char *) * (count_tokens(s, &i, &cnt) + 1));
	if (!tab)
		return (NULL);
	i = 0;
	while (s[i])
	{
		// Skip whitespace when not inside quotes
		while (s[i] && (s[i] == ' ' || s[i] == '\t'))
			i++;
		if (!s[i])
			break ;
		extract_token(&i, s, &tab, &j);
	}
	tab[j] = NULL;
	return (tab);
}
