/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_expander.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 11:34:17 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 17:15:29 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

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

static int	expand_double_q(t_expand *holder)
{
	int		i;
	char	*content;

	i = holder->indx + 2;
	while (holder->result[i] && holder->result[i] != '"')
		i++;
	if (holder->result[i] == '"' && i > holder->indx + 2)
	{
		content = ft_substr(holder->result, holder->indx + 2,
				i - holder->indx - 2);
		if (content)
		{
			handle_double_quote_content(holder, i, content);
			free(content);
			return (1);
		}
	}
	holder->indx++;
	return (1);
}

static int	expand_single_q(t_expand *holder)
{
	int		i;
	char	*content;

	i = holder->indx + 2;
	while (holder->result[i] && holder->result[i] != '\'')
		i++;
	if (holder->result[i] == '\'')
	{
		content = ft_substr(holder->result, holder->indx + 2,
				i - holder->indx - 2);
		if (content)
		{
			if (process_single_quote_content(holder, i, content))
			{
				free(content);
				return (1);
			}
			free(content);
		}
	}
	holder->indx++;
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
