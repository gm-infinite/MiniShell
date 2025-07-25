/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_replace.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 00:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 18:41:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*create_before_part(t_expand *holder)
{
	if (!holder || !holder->result || holder->indx < 0)
		return (ft_strdup(""));
	return (ft_substr(holder->result, 0, holder->indx));
}

static char	*create_after_part(t_expand *holder, int flag)
{
	if (!holder || !holder->result)
		return (ft_strdup(""));
	if (flag & 2)
	{
		if (holder->indx + 2 >= (int)ft_strlen(holder->result))
			return (ft_strdup(""));
		return (ft_strdup(&holder->result[holder->indx + 2]));
	}
	else
	{
		if (!holder->var_end)
			return (ft_strdup(""));
		return (ft_strdup(holder->var_end));
	}
}

static char	*build_expanded_string(t_expand *holder, char *before, char *after)
{
	char	*temp;
	char	*result;

	if (!before || !after || !holder->var_value)
		return (NULL);
	temp = ft_strjoin(before, holder->var_value);
	if (!temp)
		return (NULL);
	result = ft_strjoin(temp, after);
	free(temp);
	return (result);
}

static void	cleanup_replacement(char *before, char *after, int flags,
	t_expand *holder)
{
	free(before);
	free(after);
	if (flags & 1)
		free(holder->var_value);
}

int	replacevar(t_expand *holder, int flags)
{
	char	*before;
	char	*after;
	int		before_len;

	before = create_before_part(holder);
	after = create_after_part(holder, flags & 2);
	holder->expanded = build_expanded_string(holder, before, after);
	if (!holder->expanded)
	{
		cleanup_replacement(before, after, flags & 1, holder);
		return (0);
	}
	holder->var_len = ft_strlen(holder->var_value);
	before_len = ft_strlen(before);
	free(holder->result);
	cleanup_replacement(before, after, flags & 1, holder);
	holder->result = holder->expanded;
	holder->indx = before_len + holder->var_len;
	return (1);
}
