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
	int		start_len;

	start = split->start[0];
	end = split->start[split->size - 1];
	if (start == end && (size_t)(countchr_str(start, '(')
		+ countchr_str(start, ')')) == ft_strlen(start))
	{
		split->size--;
		return ;
	}
	if (start[0] == '(' && end[ft_strlen(end) - 1] == ')')
	{
		end[ft_strlen(end) - 1] = '\0';
		start_len = ft_strlen(start);
		ft_memmove(&(start[0]), &(start[1]), start_len - 1);
		start[start_len - 1] = '\0';
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
	int	vars[4];

	init_parse_vars(vars);
	while (++vars[2] <= split.size)
	{
		if (vars[2] < split.size)
			vars[1] += countchr_not_quote(split.start[vars[2]], '(');
		if (vars[1] == 0 && (vars[2] == split.size
				|| ft_strncmp("&&", split.start[vars[2]], 3) == 0
				|| ft_strncmp("||", split.start[vars[2]], 3) == 0))
		{
			if (vars[2] < split.size && split.start[vars[2]] != NULL)
				free(split.start[vars[2]]);
			if (vars[2] < split.size)
				split.start[vars[2]] = NULL;
			if (++vars[3] == -1 || (c_i[vars[3]] == '0'
					&& shell->past_exit_status == 0) || (c_i[vars[3]] == '1'
					&& shell->past_exit_status != 0))
				parser_and_or(shell, create_split(&(split.start[vars[0]]),
						vars[2] - vars[0]));
			vars[0] = vars[2] + 1;
		}
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
