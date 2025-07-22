/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_expansion_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 14:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 18:41:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_expand_holder(t_expand *holder, char *tilde_expanded)
{
	holder->result = tilde_expanded;
	holder->var_start = NULL;
	holder->var_end = NULL;
	holder->var_name = NULL;
	holder->var_value = NULL;
	holder->expanded = NULL;
	holder->indx = 0;
	holder->var_len = 0;
}

void	process_expansion_loop(t_expand *holder, t_shell *shell)
{
	int	expansion_result;

	while (holder->result[holder->indx])
	{
		if (holder->result[holder->indx] == '$')
		{
			expansion_result = handle_dollar_expansion(holder, shell);
			if (expansion_result == 0)
				break ;
			if (expansion_result == 1)
				continue ;
		}
		holder->indx++;
	}
}

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
