/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_expand.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 14:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 16:52:01 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

typedef struct s_expand
{
	char	*result;
	char	*var_start;
	char	*var_end;
	char	*var_name;
	char	*var_value;
	char	*expanded;
	int		indx;
	int		var_len;
}	t_expand;

static int	replace_var_with_value(t_expand *holder, t_shell *shell,
							int flag, int should_free_value)
{
	char	*before;
	char	*after;
	char	*temp;

	(void)shell;
	before = ft_substr(holder->result, 0, holder->indx);
	if (flag == 1)
		after = ft_strdup(&holder->result[holder->indx + 2]);
	else
		after = ft_strdup(holder->var_end);
	temp = ft_strjoin(before, holder->var_value);
	holder->expanded = ft_strjoin(temp, after);
	free(before);
	free(after);
	free(temp);
	if (!holder->expanded)
	{
		if (should_free_value)
			free(holder->var_value);
		return (0);
	}
	holder->var_len = ft_strlen(holder->var_value);
	free(holder->result);
	if (should_free_value)
		free(holder->var_value);
	holder->result = holder->expanded;
	holder->indx += holder->var_len;
	return (1);
}

static int	handle_question_mark(t_expand *holder, t_shell *shell)
{
	holder->var_value = ft_itoa(shell->past_exit_status);
	if (!holder->var_value)
		return (0);
	if (!replace_var_with_value(holder, shell, 1, 1))
		return (0);
	return (1);
}

char	*expand_tilde(char *str, t_shell *shell)
{
	char	*home;

	if (!str || str[0] != '~')
		return (ft_strdup(str));
	if (str[0] == '~' && (str[1] == '\0' || str[1] == '/'))
	{
		home = get_env_value("HOME", shell);
		if (home)
		{
			if (str[1] == '\0')
				return (ft_strdup(home));
			else
				return (ft_strjoin(home, &str[1]));
		}
	}
	return (ft_strdup(str));
}

static int	process_variable_expansion(t_expand *holder, t_shell *shell)
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
		if (!replace_var_with_value(holder, shell, 0, 0))
		{
			free(holder->var_name);
			return (0);
		}
		free(holder->var_name);
		return (1);
	}
	return (0);
}

char	*expand_variables(char *str, t_shell *shell)
{
	t_expand	holder;
	char		*tilde_expanded;

	if (!str)
		return (NULL);
	tilde_expanded = expand_tilde(str, shell);
	if (!tilde_expanded)
		return (NULL);
	holder.result = tilde_expanded;
	holder.indx = 0;
	while (holder.result[holder.indx])
	{
		if (holder.result[holder.indx] == '$')
		{
			holder.var_start = &holder.result[holder.indx + 1];
			if (*holder.var_start == '?')
			{
				if (!handle_question_mark(&holder, shell))
					break ;
				continue ;
			}
			if (*holder.var_start && (ft_isalpha(*holder.var_start)
					|| *holder.var_start == '_'))
			{
				holder.var_end = holder.var_start;
				while (*(holder.var_end) && (ft_isalnum(*(holder.var_end))
						|| *(holder.var_end) == '_'))
					holder.var_end++;
				if (!process_variable_expansion(&holder, shell))
					break ;
				continue ;
			}
		}
		holder.indx++;
	}
	return (holder.result);
}

char	*expand_variables_quoted(char *str, t_shell *shell)
{
	char	*result;
	char	*tilde_expanded;

	if (!str)
		return (NULL);
	tilde_expanded = expand_tilde(str, shell);
	if (!tilde_expanded)
		return (NULL);
	result = expand_variables(tilde_expanded, shell);
	if (result != tilde_expanded)
		free(tilde_expanded);
	return (result);
}
