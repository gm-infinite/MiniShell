/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 12:30:48 by emgenc            #+#    #+#             */
/*   Updated: 2025/01/29 16:30:11 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	ft_free_tab(char **traverse_ptr, char **tab_itself)
{
	while (traverse_ptr > tab_itself)
	{
		traverse_ptr--;
		free(*traverse_ptr);
	}
}

static int	ft_count_words(char const *s, char sep)
{
	int	word_count;

	word_count = 0;
	while (*s)
	{
		while (*s == sep)
			s++;
		if (*s)
			word_count++;
		while (*s && *s != sep)
			s++;
	}
	return (word_count);
}

static int	ft_assign(char **tab, char const *s, char sep)
{
	char		**tab_traverse;
	char const	*word_start;

	tab_traverse = tab;
	while (*s)
	{
		while (*s == sep)
			s++;
		if (*s)
		{
			word_start = s;
			while (*s && *s != sep)
				s++;
			*tab_traverse = ft_substr(word_start, 0, s - word_start);
			if (!(*tab_traverse))
			{
				ft_free_tab(tab_traverse, tab);
				return (0);
			}
			tab_traverse++;
		}
	}
	*tab_traverse = NULL;
	return (1);
}

char	**ft_split(char const *s, char c)
{
	char	**new_tab;

	new_tab = (char **)malloc(sizeof(char *) * (ft_count_words(s, c) + 1));
	if (!new_tab)
		return (NULL);
	if (!ft_assign(new_tab, s, c))
	{
		free(new_tab);
		return (NULL);
	}
	return (new_tab);
}
