/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_empty.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 13:29:14 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/02/13 14:17:26 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//checks if the string given is only composed of white-space characters
int is_empty(char *str)
{
    int i;

    if(str == NULL)
        return (1);
    i = 0;
    while (str[i] != '\0')
    {
        if (!(str[i] == ' ' || (str[i] <= 13 && str[i] >= 9)))
            return (0);
        i++;
    }
    return (1);
}