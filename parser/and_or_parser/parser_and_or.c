/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_and_or.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:07:44 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/25 11:07:45 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static int	handle_syntax_validation(t_shell *shell, t_split split)
{
	if (!check_op_errs(split)
		|| !check_par_errs(split)
		|| paranthesis_parity_check(split) == 0)
	{
		if (paranthesis_parity_check(split) == 0 && !shell->in_subshell)
			write(STDERR_FILENO,
				"bash: syntax error: unexpected end of file\n", 43);
		shell->past_exit_status = 2;
		return (1);
	}
	return (0);
}

static int	handle_parentheses_processing(t_shell *shell, t_split *split)
{
	int	was_in_subshell;

	if (check_single_par(*split) != 0)
	{
		was_in_subshell = shell->in_subshell;
		shell->in_subshell = 1;
		cut_out_par(split);
		parser_and_or(shell, *split);
		shell->in_subshell = was_in_subshell;
		return (1);
	}
	return (0);
}

void	cut_out_par(t_split *split)
{
	char	*start;
	char	*end;

	start = split->start[0];
	end = split->start[split->size - 1];
	if (is_singlepar(start, end))
	{
		split->size--;
		return ;
	}
	remove_outer_par(start, end);
	clean_empties(split);
}

static void	parse_and_or(t_shell *shell, t_split split, char *c_i)
{
	int	vars[4];

	vars[2] = -1;
	vars[1] = 0;
	vars[0] = 0;
	vars[3] = -2;
	while (++vars[2] <= split.size)
	{
		if (vars[2] < split.size)
			vars[1] += countchr_not_quote(split.start[vars[2]], '(');
		if (vars[1] == 0 && (vars[2] == split.size
				|| is_andor(split.start[vars[2]])))
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
