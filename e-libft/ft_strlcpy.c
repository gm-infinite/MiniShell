/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 12:28:53 by emgenc            #+#    #+#             */
/*   Updated: 2024/10/27 12:28:54 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize)
{
	size_t	srclen;

	if (!dstsize)
		return (ft_strlen(src));
	srclen = ft_strlen(src);
	while (*src && dstsize-- > 1)
		*dst++ = *src++;
	*dst = '\0';
	return (srclen);
}
