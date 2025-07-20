/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_parentheses.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 16:21:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 16:52:01 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static void	find_parentheses_positions(t_split cmd, int *first_idx,
			int *last_idx, int *open_count, int *close_count)
{
	int	i;
	int	j;

	i = 0;
	while (i < cmd.size)
	{
		j = 0;
		while (cmd.start[i][j] != '\0')
		{
			if (cmd.start[i][j] == '(' && *first_idx == -1)
			{
				*first_idx = i;
				(*open_count)++;
			}
			else if (cmd.start[i][j] == '(')
				(*open_count)++;
			else if (cmd.start[i][j] == ')')
			{
				(*close_count)++;
				*last_idx = i;
			}
			j++;
		}
		i++;
	}
}

static t_split	remove_single_paren_token(t_split cmd, int idx)
{
	int	i;

	i = idx;
	while (i < cmd.size - 1)
	{
		cmd.start[i] = cmd.start[i + 1];
		i++;
	}
	cmd.size--;
	return (cmd);
}

static t_split	remove_opening_paren(t_split cmd, int first_idx)
{
	if (ft_strlen(cmd.start[first_idx]) > 1)
	{
		ft_memmove(cmd.start[first_idx], cmd.start[first_idx] + 1,
			ft_strlen(cmd.start[first_idx]));
	}
	else
	{
		cmd = remove_single_paren_token(cmd, first_idx);
	}
	return (cmd);
}

static t_split	remove_closing_paren(t_split cmd, int last_idx)
{
	int	last_len;

	last_len = ft_strlen(cmd.start[last_idx]);
	if (last_len > 1)
	{
		cmd.start[last_idx][last_len - 1] = '\0';
	}
	else
	{
		cmd.size--;
	}
	return (cmd);
}

t_split	process_parentheses_in_split(t_split cmd, t_shell *shell)
{
	int	opening_parens;
	int	closing_parens;
	int	first_paren_idx;
	int	last_paren_idx;

	opening_parens = 0;
	closing_parens = 0;
	first_paren_idx = -1;
	last_paren_idx = -1;
	find_parentheses_positions(cmd, &first_paren_idx, &last_paren_idx,
		&opening_parens, &closing_parens);
	if (opening_parens == 0 || closing_parens == 0
		|| opening_parens != closing_parens)
		return (cmd);
	if (first_paren_idx != -1 && last_paren_idx != -1
		&& cmd.start[first_paren_idx][0] == '('
		&& cmd.start[last_paren_idx][ft_strlen(cmd.start[last_paren_idx])
		- 1] == ')')
	{
		if (first_paren_idx == 0 && last_paren_idx == cmd.size - 1)
		{
			cut_out_par(&cmd);
			return (process_parentheses_in_split(cmd, shell));
		}
		cmd = remove_opening_paren(cmd, first_paren_idx);
		if (first_paren_idx < last_paren_idx)
			last_paren_idx--;
		if (cmd.size == 0)
			return (cmd);
		cmd = remove_closing_paren(cmd, last_paren_idx);
		return (process_parentheses_in_split(cmd, shell));
	}
	return (cmd);
}
