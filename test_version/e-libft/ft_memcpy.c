/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 12:43:34 by emgenc            #+#    #+#             */
/*   Updated: 2024/10/27 12:43:34 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	const unsigned char	*copiedfrom;
	unsigned char		*copiedto;

	if (!dst && !src)
		return (NULL);
	copiedfrom = (const unsigned char *)src;
	copiedto = (unsigned char *)dst;
	while (n--)
		*copiedto++ = *copiedfrom++;
	return (dst);
}
