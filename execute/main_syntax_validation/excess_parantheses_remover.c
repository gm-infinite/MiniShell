/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   excess_parantheses_remover.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 15:26:09 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 17:06:09 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	handle_special_chars(t_expand *holder)
{
	if (*holder->var_start == ' ' || *holder->var_start == '/'
		|| *holder->var_start == '\t' || *holder->var_start == '\n')
	{
		holder->indx++;
		return (1);
	}
	if (*holder->var_start == '"' || *holder->var_start == '\''
		|| *holder->var_start == '$' || !ft_isprint(*holder->var_start))
	{
		holder->indx++;
		return (1);
	}
	holder->indx++;
	return (1);
}

static void	find_parentheses_positions(t_split cmd, t_paren_info *info)
{
	int	i;
	int	j;

	i = 0;
	while (i < cmd.size)
	{
		j = 0;
		while (cmd.start[i][j] != '\0')
		{
			if (cmd.start[i][j] == '(' && info->first_idx == -1)
			{
				info->first_idx = i;
				(info->open_count)++;
			}
			else if (cmd.start[i][j] == '(')
				(info->open_count)++;
			else if (cmd.start[i][j] == ')')
			{
				(info->close_count)++;
				info->last_idx = i;
			}
			j++;
		}
		i++;
	}
}

static t_split	remove_opening_paren(t_split cmd, int first_idx)
{
	int	len;
	int	i;

	if (ft_strlen(cmd.start[first_idx]) > 1)
	{
		len = ft_strlen(cmd.start[first_idx]);
		ft_memmove(cmd.start[first_idx], cmd.start[first_idx] + 1, len - 1);
		cmd.start[first_idx][len - 1] = '\0';
	}
	else
	{
		i = first_idx - 1;
		while (++i < cmd.size - 1)
			cmd.start[i] = cmd.start[i + 1];
		cmd.size--;
	}
	return (cmd);
}

static t_split	handle_parentheses_removal(t_split cmd, t_paren_info info,
	t_shell *shell)
{
	int	last_len;

	if (info.first_idx == 0 && info.last_idx == cmd.size - 1)
	{
		cut_out_par(&cmd);
		return (paren_in_split(cmd, shell));
	}
	cmd = remove_opening_paren(cmd, info.first_idx);
	if (info.first_idx < info.last_idx)
		info.last_idx--;
	if (cmd.size == 0)
		return (cmd);
	last_len = ft_strlen(cmd.start[info.last_idx]);
	if (last_len > 1)
		cmd.start[info.last_idx][last_len - 1] = '\0';
	else
		cmd.size--;
	return (paren_in_split(cmd, shell));
}

t_split	paren_in_split(t_split cmd, t_shell *shell)
{
	t_paren_info	info;

	info.open_count = 0;
	info.close_count = 0;
	info.first_idx = -1;
	info.last_idx = -1;
	find_parentheses_positions(cmd, &info);
	if (info.open_count == 0 || info.close_count == 0
		|| info.open_count != info.close_count)
		return (cmd);
	if (info.first_idx != -1 && info.last_idx != -1
		&& cmd.start[info.first_idx][0] == '('
		&& cmd.start[info.last_idx][ft_strlen(cmd.start[info.last_idx])
		- 1] == ')')
	{
		return (handle_parentheses_removal(cmd, info, shell));
	}
	return (cmd);
}
