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

//second half of check_single_par
int check_single_par2(t_split split, int word_number, int *count, int *decreased)
{
	int j;

	j = 0;
	while(split.start[word_number][j] != '\0')
		{
			if(split.start[word_number][j] == '(')
				(*count)++;
			if(split.start[word_number][j] == ')')
				(*count)--;
			if ((*count) == 0)
				(*decreased)++;
			if ((*decreased) != 0 && split.start[word_number][j] == '(')
				return (-1);
			j++;
		}
	return (1);
}

//only send a 0 to zero, its here because of norminette constraints
int check_single_par(t_split split)
{
	int count;
	int word_number;
	int decreased;

	word_number = 0;
	decreased = 0;
	count = 0;
	while (word_number < split.size)
	{
		if (check_single_par2(split, word_number, &count, &decreased) < 0)
			return (0);
		word_number++;
	}
	return (1);
}

//finds str given in split's string array, it searches for exact match
int check_symbol(t_split split, char *find)
{
	int count;
	int i;

	i = 0;
	count = 0;
	while (i < split.size)
	{
		count = count + countchr_not_quote(split.start[i], '(');
		if (count == 0)
		{
			if (ft_strncmp(split.start[i], find, ft_strlen(find)) == 0)
				return (i);
		}
		count = count - countchr_not_quote(split.start[i], ')');
		i++;
	}
	return (-2);
}
//for test
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

void cut_out_par(t_split *split)
{
	char *start;
	char *end;

	start = split->start[0];
	end = split->start[split->size - 1];
	while (start[0] == '(' && end[ft_strlen(end) - 1] == ')')
	{
		ft_memmove(&(start[0]), &(start[1]), ft_strlen(start));
		end[ft_strlen(end) - 1] = '\0';
		if (start[0] == '\0')
		{
			split->start = &(split->start[1]);
			split->size--;
		}
		if (end[0] == '\0')
			split->size--;
		start = split->start[0];
		end = split->start[split->size - 1];
	}
}

void parser_and_or(t_shell *shell, t_split split)
{
	static int deepness = -1;
	int cut_index;
	deepness++;
	if (check_single_par(split) != 0)
		cut_out_par(&split);
	cut_index = check_symbol(split, "||");
	if (cut_index < 0)
		cut_index = check_symbol(split, "&&");
	if (cut_index >= 0)
	{
		if (cut_index > 0)
			parser_and_or(shell, (create_split(split.start, cut_index)));
		if (cut_index < split.size)
			parser_and_or(shell, (create_split(&(split.start[cut_index + 1]), split.size - cut_index - 1)));
	}
		printf("deepness:%d", deepness);
		print_result(split);
	deepness--;
	(void)shell;
	if (shell->split_input.size == split.size)
		deepness = -1;
}