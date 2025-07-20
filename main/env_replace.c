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
	return (ft_substr(holder->result, 0, holder->indx));
}

static char	*create_after_part(t_expand *holder, int flag)
{
	if (flag == 1)
		return (ft_strdup(&holder->result[holder->indx + 2]));
	else
		return (ft_strdup(holder->var_end));
}

static char	*build_expanded_string(t_expand *holder, char *before, char *after)
{
	char	*temp;
	char	*result;

	temp = ft_strjoin(before, holder->var_value);
	if (!temp)
		return (NULL);
	result = ft_strjoin(temp, after);
	free(temp);
	return (result);
}

static void	cleanup_replacement(char *before, char *after, int should_free_value, t_expand *holder)
{
	free(before);
	free(after);
	if (should_free_value)
		free(holder->var_value);
}

int	replace_var_with_value(t_expand *holder, t_shell *shell,
						int flag, int should_free_value)
{
	char	*before;
	char	*after;

	(void)shell;
	before = create_before_part(holder);
	after = create_after_part(holder, flag);
	holder->expanded = build_expanded_string(holder, before, after);
	if (!holder->expanded)
	{
		cleanup_replacement(before, after, should_free_value, holder);
		return (0);
	}
	holder->var_len = ft_strlen(holder->var_value);
	free(holder->result);
	cleanup_replacement(before, after, should_free_value, holder);
	holder->result = holder->expanded;
	holder->indx += holder->var_len;
	return (1);
}
