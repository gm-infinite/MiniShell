/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 14:02:22 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/06/01 15:50:55 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wildcard_handler.h"

int ft_strrcmp(const char *s1, const char *s2)
{
	int s1i;
	int s2i;

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

char *ft_strjoin_sq_f(char **to_free, const char *to_add)
{
	char *ret;
	int ret_size;

	
	if (*to_free == NULL)
	{
		ret_size = ft_strlen(to_add) + 3;
		ret = (char *)ft_calloc(ret_size, sizeof(char));
		if (ret == NULL)
			return (NULL);
		ft_strlcpy(ret, "'", ret_size);
	}
	else
	{
		ret_size = ft_strlen(*to_free) + ft_strlen(to_add) + 4;
		ret = (char *)ft_calloc(ret_size, sizeof(char));
		if (ret == NULL)
			return (NULL);
		ft_strlcpy(ret, *to_free, ret_size);
		ft_strlcat(ret, " '", ret_size);
		free(*to_free);
	}
	ft_strlcat(ret, to_add, ret_size);
	ft_strlcat(ret, "'", ret_size);
	return (ret);
}