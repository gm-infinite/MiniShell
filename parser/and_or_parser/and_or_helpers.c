/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   and_or_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 00:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/22 15:59:05 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

void	init_parse_vars(int *vars)
{
	vars[2] = -1;
	vars[1] = 0;
	vars[0] = 0;
	vars[3] = -2;
}

int	handle_syntax_validation(t_shell *shell, t_split split)
{
	if (!check_operator_syntax_errors(split)
		|| !check_parentheses_syntax_errors(split)
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

int	handle_parentheses_processing(t_shell *shell, t_split *split)
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
