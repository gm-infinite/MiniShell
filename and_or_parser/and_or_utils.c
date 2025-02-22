/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   and_or_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 18:55:15 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/02/22 20:26:42 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

//finds str given in split's string array, it searches for exact match, flag 1 ignores phrases in paranthesis
int check_symbol(t_split split, char *find, int flag)
{
	int count;
	int i;

	i = 0;
	count = 0;
	while (i < split.size)
	{
		count = count + countchr_not_quote(split.start[i], '(');
		if (flag == 0 || count == 0)
		{
			if (ft_strncmp(split.start[i], find, ft_strlen(find)) == 0)
				return (i);
		}
		count = count - countchr_not_quote(split.start[i], ')');
		i++;
	}
	return (-2);
}

//counts the amouth of char c in string str
int countchr_str(char*str,  char c)
{
	int j;
	int count;

	j = 0;
	count = 0;
	while(str[j] != '\0')
	{
		if (str[j] == c)
			count++;
		j++;
	}
	return (count);
}

//counts the char c in string str only if its in "..."
int countchr_quote(char*str,  char c)
{
	int j;
	int inquote;
	int count;

	j = 0;
	count = 0;
	inquote = 0;
	while(str[j] != '\0')
	{
		if (str[j] == '"')
		inquote = inquote ^ 1;
		if (inquote == 1 && str[j] == c)
			count++;
		j++;
	}
	return (count);
}

//counts the char c in string only if its NOT in "..."
int countchr_not_quote(char*str,  char c)
{
	return (countchr_str(str, c) - countchr_quote(str, c));
}


//checks the paranthesis on the outside is for each other
int check_single_par(t_split split)
{
	char *start;
    char *end;

    start = split.start[0];
    end = split.start[split.size - 1];
    if (start[0] == '(' && end[ft_strlen(end) - 1] == ')' && \
    check_symbol(split, "||", 1) < 0 && check_symbol(split, "&&", 1) < 0)
        return (1);
    return (0);
}