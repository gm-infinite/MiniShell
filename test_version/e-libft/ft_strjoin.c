/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 12:29:27 by emgenc            #+#    #+#             */
/*   Updated: 2025/01/29 16:38:14 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*newstring;
	size_t	len1;
	size_t	len2;

	len1 = ft_strlen((const char *)s1);
	len2 = ft_strlen((const char *)s2);
	newstring = (char *)malloc((sizeof(char) * (len1 + len2 + 1)));
	if (!newstring)
		return (NULL);
	ft_strlcpy(newstring, s1, len1 + 1);
	ft_strlcat(newstring, s2, len1 + len2 + 1);
	return (newstring);
}
