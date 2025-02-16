/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   t_split_utils.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:53:30 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/02/13 14:27:14 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "t_split.h"

//find's first the exact string given in the split char**
//return's the position if found
//return -1 if theres no match
//return -2 if theres an error
int strnsplit(t_split split, char *find)
{
    int i;

    if (find == NULL || *find == '\0')
        return (-2);
    i = 0;
    while (i < split.size)
    {
        if (ft_strncmp(split.start[i], find, ft_strlen(find)) == 0)
            return (i);
        i++;
    }
    return (-1);
}

//creates a new split with a char** and size given.
//it uses start paramater given to set char**
//it uses size paramater to set int
t_split create_split(char **start, int size)
{
    t_split ret;

    ret.start = start;
    ret.size = size;
    return (ret);
}

//creates a t_split struct from a string given
//it uses ft_split to get char**
//uses the char** to count the elements to set the int
//is a malloc error happenes, size is set to -1
t_split create_split_str(char *str)
{
    t_split ret;
    int i;

    ret.start = ft_split(str, ' ');
    if(ret.start == NULL)
    {
        ret.size = -1;
        return (ret);
    }
    i = 0;
    while(ret.start[i] != NULL)
        i++;
    ret.size = i;
    return (ret);
}

//free's the t_split calue given
//free's every element of char** and sets char** to NULL
//sets int to 0
void free_split(t_split *split)
{
    int i;
    
    i = 0;
    while (i < split->size)
    {
        if (split->start[i] != NULL)
            free(split->start[i]);
        i++;
    }
    free(split->start);
    split->start = NULL;
    split->size = 0;
}