/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 12:27:36 by emgenc            #+#    #+#             */
/*   Updated: 2024/10/27 12:27:37 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char	*array;
	size_t	len;
	size_t	i;

	i = 0;
	if (!s)
		return (ft_strdup(""));
	len = ft_strlen(s);
	array = (char *)malloc(sizeof(char) * (len + 1));
	if (!array)
		return (NULL);
	while (i < len)
	{
		array[i] = (*f)(i, s[i]);
		++i;
	}
	array[i] = 0;
	return (array);
}
