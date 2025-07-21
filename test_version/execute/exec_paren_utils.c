/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_paren_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 23:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 23:46:49 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static t_split	remove_single_paren_token(t_split cmd, int index)
{
	int	i;

	i = index;
	while (i < cmd.size - 1)
	{
		cmd.start[i] = cmd.start[i + 1];
		i++;
	}
	cmd.size--;
	return (cmd);
}

t_split	remove_opening_paren(t_split cmd, int first_idx)
{
	int	len;

	if (ft_strlen(cmd.start[first_idx]) > 1)
	{
		len = ft_strlen(cmd.start[first_idx]);
		ft_memmove(cmd.start[first_idx], cmd.start[first_idx] + 1, len - 1);
		cmd.start[first_idx][len - 1] = '\0';
	}
	else
	{
		cmd = remove_single_paren_token(cmd, first_idx);
	}
	return (cmd);
}

t_split	remove_closing_paren(t_split cmd, int last_idx)
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

t_split	handle_parentheses_removal(t_split cmd, t_paren_info info,
		t_shell *shell)
{
	if (info.first_idx == 0 && info.last_idx == cmd.size - 1)
	{
		cut_out_par(&cmd);
		return (process_parentheses_in_split(cmd, shell));
	}
	cmd = remove_opening_paren(cmd, info.first_idx);
	if (info.first_idx < info.last_idx)
		info.last_idx--;
	if (cmd.size == 0)
		return (cmd);
	cmd = remove_closing_paren(cmd, info.last_idx);
	return (process_parentheses_in_split(cmd, shell));
}
