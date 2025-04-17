/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_quotes.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 16:30:24 by emgenc            #+#    #+#             */
/*   Updated: 2025/04/08 19:48:15 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "t_split.h"

static int	count_tokens(const char *s, int *i, int *cnt)
{
	char	q;

	while (s[*i])
	{
		while (s[*i] == ' ' || s[*i] == '\t')
			(*i)++;
		if (!s[*i])
			break ;
		(*cnt)++;
		if (s[*i] == '\'' || s[*i] == '\"')
		{
			q = s[(*i)++];
			while (s[*i] && s[*i] != q)
				(*i)++;
			if (s[*i])
				(*i)++;
		}
		else
			while (s[*i] && s[*i] != ' ' && s[*i] != '\t')
				(*i)++;
	}
	return (*cnt);
}

static void	quote_seperator(int *i, const char *s, char ***tab, int *j)
{
	int		start;
	char	q;

	q = s[(*i)++];
	start = (*i) - 1;
	while (s[*i] && s[*i] != q)
		(*i)++;
	(*tab)[(*j)++] = ft_substr(s, start, (*i) - start + 1);
	if (s[*i])
		(*i)++;
}

static void	normal_splitter(int *i, const char *s, char ***tab, int *j)
{
	int	start;

	start = *i;
	while (s[*i] && s[*i] != ' ' && s[*i] != '\t')
		(*i)++;
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
		while (s[i] == ' ' || s[i] == '\t')
			i++;
		if (!s[i])
			break ;
		if (s[i] == '\'' || s[i] == '\"')
			quote_seperator(&i, s, &tab, &j);
		else
			normal_splitter(&i, s, &tab, &j);
	}
	tab[j] = NULL;
	return (tab);
}
