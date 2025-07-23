/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_and_or_process_utils.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:15:24 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/21 01:00:18 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	is_and_or_operator(char *str)
{
	if (!str)
		return (0);
	return (ft_strncmp("&&", str, 3) == 0 || ft_strncmp("||", str, 3) == 0);
}

int	should_execute_segment(int operator_index, char *c_i, t_shell *shell)
{
	if (operator_index == -1)
		return (1);
	if (c_i[operator_index] == '0' && shell->past_exit_status == 0)
		return (1);
	if (c_i[operator_index] == '1' && shell->past_exit_status != 0)
		return (1);
	return (0);
}

void	process_segment(t_shell *shell, t_split split, int *vars, char *c_i)
{
	if (vars[2] < split.size && split.start[vars[2]] != NULL)
		free(split.start[vars[2]]);
	if (vars[2] < split.size)
		split.start[vars[2]] = NULL;
	vars[3]++;
	if (should_execute_segment(vars[3], c_i, shell))
		parser_and_or(shell, create_split(&(split.start[vars[0]]),
				vars[2] - vars[0]));
	vars[0] = vars[2] + 1;
}
