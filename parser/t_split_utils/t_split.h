/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   t_split.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:53:30 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/16 20:58:00 by emgenc           ###   ########.fr       */
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
int		strnsplit(t_split split, char *find);
void	rearrange_split(t_split *split);
char	**ft_split_quotes(const char *s);
char	*revert_split_str(t_split split);

#endif