/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:02:44 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/25 12:13:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wildcard_handler.h"

int	ft_strrcmp(const char *s1, const char *s2)
{
	int	s1i;
	int	s2i;

	s1i = ft_strlen(s1) - 1;
	s2i = ft_strlen(s2) - 1;
	if (s2i < s1i)
		return (1);
	while (s1i > 0)
	{
		if (s1[s1i] != s2[s2i])
			return (s1[s1i] - s2[s2i]);
		s2i--;
		s1i--;
	}
	return (s1[0] - s2[s2i]);
}
