/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   t_split.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:53:30 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/02/13 14:27:14 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef T_SPLIT_H
# define T_SPLIT_H
# include "../e-libft/libft.h"

typedef struct s_split
{
	char	**start;
	int		size;
}			t_split;

t_split	create_split_str(char *str);
void	free_split(t_split *split);
t_split	create_split(char **start, int size);
int		strnsplit(t_split split, char *find);

#endif