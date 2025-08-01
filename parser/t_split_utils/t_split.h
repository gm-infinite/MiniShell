/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   t_split.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:04:55 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/25 12:28:53 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef T_SPLIT_H
# define T_SPLIT_H
# include "../../e-libft/libft.h"

typedef struct s_split
{
	char	**start;
	int		size;
}			t_split;

t_split	create_split_str(char *str);
void	free_split(t_split *split);
t_split	create_split(char **start, int size);
char	**ft_split_quotes(const char *s);
char	*revert_split_str(t_split split);

#endif