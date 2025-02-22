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

//for test, it prints the split struct with detail.
static void	print_result(t_split test)
{
	int	i;

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
//cuts out the outer most (), if the cutting made the string empty, it removes it from split
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
//parser for && and || with () priority
void	parser_and_or(t_shell *shell, t_split split)
{
	static int	deepness = -1;
	int			cut_index;
	
	if (split.size <= 0 || split.start == NULL)
		return ;
	if (check_single_par(split) != 0)
	{
		cut_out_par(&split);
		parser_and_or(shell, split);
		return ;
	}

	deepness++;
	cut_index = check_symbol(split, "||", 1);
	if (cut_index < 0)
		cut_index = check_symbol(split, "&&", 1);
	if (cut_index >= 0)
	{
		if (cut_index > 0)
			parser_and_or(shell, (create_split(split.start, cut_index)));
		if (cut_index < split.size)
			parser_and_or(shell, (create_split(&(split.start[cut_index + 1]), split.size - cut_index - 1)));
	}
	else
	{
		// | parser
	}
		printf("deepness:%d", deepness);
		print_result(split);
	deepness--;
	(void)shell;
	if (shell->split_input.size == split.size)
		deepness = -1;
}
