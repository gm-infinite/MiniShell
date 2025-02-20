/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_and_or.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:15:24 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/02/20 19:15:24 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int count_paranthesis(char *str)
{
	int i;
	int count;

	i = 0;
	count = 0;
	while (str[i] != '\0')
	{
		if (str[i] == '(')
			count++;
		else if (str[i] == ')')
			count--;
		i++;
	}
	printf("count:%d\n", count);
	return count;
	
}

int count_par(t_split split, int word_number, int o_c)
{
	int j;
	int count;

	j = 0;
	count = 0;
	while(split.start[word_number][j] != '\0')
	{
		if (o_c == 1 && split.start[word_number][j] == '(')
			count++;
		if (o_c == 2 && split.start[word_number][j] == ')')
			count--;
		j++;
	}
	return (count);
}

int check_symbol(t_split split, char *find)
{
	int count;
	int i;

	i = 0;
	count = 0;
	while (i < split.size)
	{
		count = count + count_par(split, i, 1);
		if (count == 0)
		{
			if (ft_strncmp(split.start[i], find, ft_strlen(find)) == 0)
				return (i);
		}
		count = count + count_par(split, i, 2);
		i++;
	}
	return (-1);
}

void print_result(t_split test)
{
    int i;
    printf("\n\tsize: %d", test.size);
    printf("\n\tstart:%3p", test.start);
    i = 0;
    while(i <= test.size)
    {
        printf("\n\t\t%-2d:%s", i, test.start[i]);
        i++;
    }
    printf(" -> after last value\n\n");
}

void parser_and_or(t_shell *shell, t_split split)
{
	static int deepness = -1;
	int cut_index;
	deepness++;
	cut_index = check_symbol(split, "||");
	if (cut_index < 0)
		cut_index = check_symbol(split, "&&");
	if (cut_index >= 0)
	{
		if (cut_index > 0)
			parser_and_or(shell, (create_split(split.start, cut_index)));
		if (cut_index < split.size - 1)
			parser_and_or(shell, (create_split(&(split.start[cut_index + 1]), split.size - cut_index - 1)));
	}
	printf("deepness:%d", deepness);
	print_result(split);
	deepness--;
	(void)shell;
	if (shell->split_input.size == split.size)
		deepness = -1;
}