/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 12:25:30 by emgenc            #+#    #+#             */
/*   Updated: 2025/02/13 23:22:44 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*to_be_returned;
	size_t	size;

	if (!s)
		return (NULL);
	if (start >= ft_strlen(s))
		return (ft_strdup(""));
	else
	{
		size = ft_strlen(s + start);
		if (size < len)
			len = size;
		to_be_returned = (char *)malloc((sizeof(char) * len) + 1);
		if (!to_be_returned)
			return (NULL);
		ft_strlcpy(to_be_returned, &s[start], len + 1);
		return (to_be_returned);
	}
}
