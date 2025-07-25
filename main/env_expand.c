/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_expand.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 14:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 18:41:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_question_mark(t_expand *holder, t_shell *shell)
{
	holder->var_value = ft_itoa(shell->past_exit_status);
	if (!holder->var_value)
		return (0);
	if (!replacevar(holder, 3))
		return (0);
	return (1);
}

static int	process_var_expansion_internal(t_expand *holder, t_shell *shell)
{
	if (holder->var_end > holder->var_start)
	{
		holder->var_name = ft_substr(holder->var_start, 0,
				holder->var_end - holder->var_start);
		if (!holder->var_name)
			return (0);
		holder->var_value = get_env_value(holder->var_name, shell);
		if (!holder->var_value)
			holder->var_value = "";
		if (!replacevar(holder, 0))
		{
			free(holder->var_name);
			return (0);
		}
		free(holder->var_name);
		return (1);
	}
	return (0);
}

static int	handle_var_name_expansion(t_expand *holder, t_shell *shell)
{
	holder->var_end = holder->var_start;
	while (*(holder->var_end) && (ft_isalnum(*(holder->var_end))
			|| *(holder->var_end) == '_'))
		holder->var_end++;
	if (!process_var_expansion_internal(holder, shell))
		return (0);
	return (1);
}

int	expand_dollar(t_expand *holder, t_shell *shell)
{
	if (holder->indx + 1 >= (int)ft_strlen(holder->result))
	{
		holder->indx++;
		return (1);
	}
	holder->var_start = &holder->result[holder->indx + 1];
	if (*holder->var_start == '"')
		return (expand_double_q(holder));
	if (*holder->var_start == '\'')
		return (expand_single_q(holder));
	if (*holder->var_start == '?')
	{
		if (!handle_question_mark(holder, shell))
			return (0);
		return (1);
	}
	if (*holder->var_start && (ft_isalpha(*holder->var_start)
			|| *holder->var_start == '_'))
		return (handle_var_name_expansion(holder, shell));
	return (handle_special_chars(holder));
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
	init_expand_holder(&holder, tilde_expanded);
	expander_loop(&holder, shell);
	return (holder.result);
}
