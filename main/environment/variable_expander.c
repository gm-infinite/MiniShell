/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   variable_expander.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:28:06 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 14:29:26 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	expander_loop(t_expand *holder, t_shell *shell)
{
	int	expansion_result;

	while (holder->result[holder->indx])
	{
		if (holder->result[holder->indx] == '$')
		{
			expansion_result = expand_dollar(holder, shell);
			if (expansion_result == 0)
				break ;
			if (expansion_result == 1)
				continue ;
		}
		holder->indx++;
	}
}

char	*expandvar(char *str, t_shell *shell)
{
	t_expand	holder;
	char		*tilde_expanded;

	if (!str)
		return (NULL);
	tilde_expanded = tilde(str, shell);
	if (!tilde_expanded)
		return (NULL);
	holder.result = tilde_expanded;
	holder.var_start = NULL;
	holder.var_end = NULL;
	holder.var_name = NULL;
	holder.var_value = NULL;
	holder.expanded = NULL;
	holder.indx = 0;
	holder.var_len = 0;
	expander_loop(&holder, shell);
	return (holder.result);
}
