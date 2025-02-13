/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 12:26:16 by emgenc            #+#    #+#             */
/*   Updated: 2024/10/27 12:26:18 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char	*pointer;

	pointer = NULL;
	while (*s)
	{
		if (*s == (unsigned char)c)
			pointer = (char *)s;
		s++;
	}
	if ((unsigned char)c == 0)
	{
		pointer = (char *)s;
		return (pointer);
	}
	return (pointer);
}
