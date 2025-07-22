/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_expand_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 14:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 18:41:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_variables_quoted(char *str, t_shell *shell)
{
	char	*result;
	char	*tilde_expanded;

	if (!str)
		return (NULL);
	tilde_expanded = expand_tilde(str, shell);
	if (!tilde_expanded)
		return (NULL);
	result = expand_with_quotes(tilde_expanded, shell);
	if (result != tilde_expanded && tilde_expanded)
		free(tilde_expanded);
	return (result);
}

static void	init_quote_expansion(t_expand *holder, char *tilde_expanded,
			int *in_single_quotes, int *in_double_quotes)
{
	holder->result = tilde_expanded;
	holder->var_start = NULL;
	holder->var_end = NULL;
	holder->var_name = NULL;
	holder->var_value = NULL;
	holder->expanded = NULL;
	holder->indx = 0;
	holder->var_len = 0;
	*in_single_quotes = 0;
	*in_double_quotes = 0;
}

static void	process_quote_expansion_loop(t_expand *holder, t_shell *shell,
			int *in_single_quotes, int *in_double_quotes)
{
	int	expansion_result;

	while (holder->result[holder->indx])
	{
		if (holder->result[holder->indx] == '\'' && !*in_double_quotes)
			*in_single_quotes = !*in_single_quotes;
		else if (holder->result[holder->indx] == '"' && !*in_single_quotes)
			*in_double_quotes = !*in_double_quotes;
		else if (holder->result[holder->indx] == '$' && !*in_single_quotes)
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

char	*expand_with_quotes(char *str, t_shell *shell)
{
	t_expand	holder;
	char		*tilde_expanded;
	int			in_single_quotes;
	int			in_double_quotes;

	if (!str)
		return (NULL);
	tilde_expanded = expand_tilde(str, shell);
	if (!tilde_expanded)
		return (NULL);
	init_quote_expansion(&holder, tilde_expanded,
		&in_single_quotes, &in_double_quotes);
	process_quote_expansion_loop(&holder, shell,
		&in_single_quotes, &in_double_quotes);
	return (holder.result);
}
