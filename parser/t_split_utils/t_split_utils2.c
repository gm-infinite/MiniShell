/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   t_split_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:03:51 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/25 11:04:43 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "t_split.h"

char	*revert_split_str(t_split split)
{
	int		i;
	char	*ret;
	int		ret_size;

	i = 0;
	ret_size = 0;
	if (split.size < 1 || split.start == NULL)
		return (NULL);
	while (i < split.size)
	{
		ret_size += ft_strlen(split.start[i]) + 1;
		i++;
	}
	ret = (char *)ft_calloc(ret_size, sizeof(char));
	if (ret == NULL)
		return (NULL);
	i = 1;
	ft_strlcpy(ret, split.start[0], ret_size);
	while (i < split.size)
	{
		ft_strlcat(ret, " ", ret_size);
		ft_strlcat(ret, split.start[i], ret_size);
		i++;
	}
	return (ret);
}
