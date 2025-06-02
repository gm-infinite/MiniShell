/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_and_or.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:15:24 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/06/02 13:23:48 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

void print_result(t_split *test)
{
    int i;
    printf("\n\tsize: %d", test->size);
    printf("\n\tstart:%3p", test->start);
    i = 0;
    while(i <= test->size)
    {
        printf("\n\t\t%-2d:%s", i, test->start[i]);
        i++;
    }
    printf(" -> after last value\n");
}

// Cuts out the outermost (). New string == empty? Remove it from 'split'
static void	cut_out_par(t_split *split)
{
	char	*start;
	char	*end;

	start = split->start[0];
	end = split->start[split->size - 1];
	if (start == end && (size_t)(countchr_str(start, '(') + \
		countchr_str(start, ')')) == ft_strlen(start))
	{
		split->size--;
		return ;
	}
	if (start[0] == '(' && end[ft_strlen(end) - 1] == ')')
	{
		end[ft_strlen(end) - 1] = '\0';
		ft_memmove(&(start[0]), &(start[1]), ft_strlen(start));
	}
	if (split->size > 1 && start[0] == '\0')
	{
		split->start = &(split->start[1]);
		split->size--;
	}
	if (end[0] == '\0')
		split->size--;
}

// counts exact str in split's str array . if flag = 1 does not count str in paranthesis
int	count_str_split(t_split split, const char *str, int flag)
{
	int i;
	int count;
	int par;

	i = 0;
	count = 0;
	par = 0;
	while (i < split.size)
	{
		par += countchr_not_quote(split.start[i], '(');
		if ((flag == 0 || par == 0) && (strncmp(str, split.start[i], 4) == 0))
			count++;
		par -= countchr_not_quote(split.start[i], ')');
		i++;
	}
	return (count);
}

char *get_cut_indexs(t_split split)
{
	int i;
	int par;
	char *ret;
	int check;

	i = 0;
	par = 0;
	check = 0;
	ret = ft_calloc(count_str_split(split, "||", 1) + count_str_split(split, "&&", 1) + 1, sizeof(char));
	if (ret == NULL)
		return (NULL);
	while (i < split.size)
	{
		par += countchr_not_quote(split.start[i], '(');
		if (par == 0) 
		{
			if(strncmp("||", split.start[i], 3) == 0)
				ret[check++] = '1';
			else if (strncmp("&&", split.start[i], 3) == 0)
				ret[check++] = '0';
		}
		par -= countchr_not_quote(split.start[i], ')');
		printf("%d-%s ",i ,ret);
		i++;
	}
	return (ret);
}

void parse_and_or(t_shell *shell, t_split split, char *cut_indexs)
{
	int i;
	int par;
	int start;
	int check;

	i = 0;
	par = 0;
	start = 0;
	check = -1;
	while (i <= split.size)
	{
		par += countchr_not_quote(split.start[i], '(');
		if ((par == 0) && ((i == split.size) || (strncmp("&&", split.start[i], 4) == 0) || (strncmp("||", split.start[i], 4) == 0)))
		{
			if (split.start[i] != NULL)
				free(split.start[i]);
			split.start[i] = NULL;
			if (check == -1 || \
				(cut_indexs[check] == '0' && shell->past_exit_status == 0) || \
				(cut_indexs[check] == '1' && shell->past_exit_status != 0))
				parser_and_or(shell, create_split(&(split.start[start]), i - start));
			start = i + 1;
			check++;
		}
		par -= countchr_not_quote(split.start[i], ')');
		i++;
	}
}

// Parser for && and || with () priority
void	parser_and_or(t_shell *shell, t_split split)
{
	char	*cut_indexs;

	print_result(&split);
	if (split.size <= 0 || split.start == NULL)
		return ;
	if (check_single_par(split) != 0)
	{
		cut_out_par(&split);
		parser_and_or(shell, split);
		return ;
	}
	if (count_str_split(split, "||", 1) + count_str_split(split, "&&", 1) > 0)
	{
	cut_indexs = get_cut_indexs(split);
	parse_and_or(shell, split, cut_indexs);
	free(cut_indexs);
	}
	else
		{/*executer*/}
}
