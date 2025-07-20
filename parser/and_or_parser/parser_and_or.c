/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_and_or.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:15:24 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/20 22:15:10 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

// Cuts out the outermost (). New string == empty? Remove it from 'split'
void	cut_out_par(t_split *split)
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

void	parse_and_or(t_shell *shell, t_split split, char *c_i)
{
	int	i;
	int	par;
	int	st;
	int	check;

	i = -1;
	par = 0;
	st = 0;
	check = -2;
	while (++i <= split.size)
	{
		if (i < split.size)
			par += countchr_not_quote(split.start[i], '(');
		if (par == 0 && (i == split.size || ft_strncmp("&&", split.start[i], \
			3) == 0 || ft_strncmp("||", split.start[i], 3) == 0))
		{
			if (i < split.size && split.start[i] != NULL)
				free(split.start[i]);
			if (i < split.size)
				split.start[i] = NULL;
			if (++check == -1 || (c_i[check] == '0' && shell->past_exit_status \
			== 0) || (c_i[check] == '1' && shell->past_exit_status != 0))
				parser_and_or(shell, create_split(&(split.start[st]), i - st));
			st = i + 1;
		}
		if (i < split.size)
			par -= countchr_not_quote(split.start[i], ')');
	}
}

void	parser_and_or(t_shell *shell, t_split split)
{
	char	*cut_indexs;

	if (split.size <= 0 || split.start == NULL)
		return ;
	if (!check_operator_syntax_errors(split) || !check_parentheses_syntax_errors(split) || paranthesis_parity_check(split) == 0)
	{
        if (paranthesis_parity_check(split) == 0)
            write(STDERR_FILENO, "bash: syntax error: unexpected end of file1\n", 44);
        shell->past_exit_status = 2;
        return ;
    }
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
		shell->past_exit_status = execute_command(split, shell);
}
