/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 12:29:09 by emgenc            #+#    #+#             */
/*   Updated: 2024/10/27 12:29:10 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	dstlen;
	size_t	srclen;

	dstlen = 0;
	while (dstlen < dstsize && *dst)
	{
		dstlen++;
		dst++;
	}
	if (dstlen == dstsize)
		return (dstlen + ft_strlen(src));
	srclen = 0;
	while (src[srclen])
	{
		if (srclen < dstsize - dstlen - 1)
			*dst++ = src[srclen];
		srclen++;
	}
	*dst = '\0';
	return (srclen + dstlen);
}
