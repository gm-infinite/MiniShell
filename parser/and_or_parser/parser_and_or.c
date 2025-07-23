/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_and_or.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:15:24 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/21 01:00:18 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

void	cut_out_par(t_split *split)
{
	char	*start;
	char	*end;

	start = split->start[0];
	end = split->start[split->size - 1];
	if (is_single_parentheses_string(start, end))
	{
		split->size--;
		return ;
	}
	remove_outer_parentheses(start, end);
	cleanup_empty_elements(split);
}

void	parse_and_or(t_shell *shell, t_split split, char *c_i)
{
	int	vars[4];

	init_parse_vars(vars);
	while (++vars[2] <= split.size)
	{
		if (vars[2] < split.size)
			vars[1] += countchr_not_quote(split.start[vars[2]], '(');
		if (vars[1] == 0 && (vars[2] == split.size
				|| is_and_or_operator(split.start[vars[2]])))
			process_segment(shell, split, vars, c_i);
		if (vars[2] < split.size)
			vars[1] -= countchr_not_quote(split.start[vars[2]], ')');
	}
}

void	parser_and_or(t_shell *shell, t_split split)
{
	char	*cut_indexs;

	if (split.size <= 0 || split.start == NULL)
		return ;
	if (handle_syntax_validation(shell, split))
		return ;
	if (handle_parentheses_processing(shell, &split))
		return ;
	if (count_str_split(split, "||", 1) + count_str_split(split, "&&", 1) > 0)
	{
		cut_indexs = get_cut_indexs(split);
		parse_and_or(shell, split, cut_indexs);
		free(cut_indexs);
	}
	else
		shell->past_exit_status = execute_command(split, shell);
}
