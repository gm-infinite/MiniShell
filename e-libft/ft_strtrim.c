/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 12:26:04 by emgenc            #+#    #+#             */
/*   Updated: 2024/10/27 12:26:05 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*trimmed_string;
	size_t	begin;
	size_t	end;

	if (!s1)
		return (NULL);
	if (!set)
		return (ft_strdup(s1));
	end = ft_strlen(s1) - 1;
	begin = 0;
	while (s1[begin] && ft_strchr(set, s1[begin]))
		begin++;
	while (s1[end] && ft_strchr(set, s1[end]))
		end--;
	trimmed_string = ft_substr(s1, begin, end - begin + 1);
	return (trimmed_string);
}
