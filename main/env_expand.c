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
	if (!replace_var_with_value(holder, shell, 3))
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
		if (!replace_var_with_value(holder, shell, 0))
		{
			free(holder->var_name);
			return (0);
		}
		free(holder->var_name);
		return (1);
	}
	return (0);
}

static int	handle_dollar_expansion(t_expand *holder, t_shell *shell)
{
	if (holder->indx + 1 >= (int)ft_strlen(holder->result))
		return (-1);
	holder->var_start = &holder->result[holder->indx + 1];
	
	// Handle $"string" locale translation syntax - just skip the $
	if (*holder->var_start == '"')
	{
		// Remove the $ by shifting the rest of the string left
		ft_memmove(&holder->result[holder->indx], 
				&holder->result[holder->indx + 1], 
				ft_strlen(&holder->result[holder->indx + 1]) + 1);
		// Now find the closing quote and skip to after it
		holder->indx++; // Skip the opening quote
		while (holder->result[holder->indx] && holder->result[holder->indx] != '"')
			holder->indx++;
		if (holder->result[holder->indx] == '"')
			holder->indx++; // Skip the closing quote
		return (1);
	}
	
	if (*holder->var_start == '?')
	{
		if (!handle_question_mark(holder, shell))
			return (0);
		return (1);
	}
	if (*holder->var_start && (ft_isalpha(*holder->var_start)
			|| *holder->var_start == '_'))
	{
		holder->var_end = holder->var_start;
		while (*(holder->var_end) && (ft_isalnum(*(holder->var_end))
				|| *(holder->var_end) == '_'))
			holder->var_end++;
		if (!process_var_expansion_internal(holder, shell))
			return (0);
		return (1);
	}
	// Lone $ not followed by valid variable name - remove it
	ft_memmove(&holder->result[holder->indx], 
			&holder->result[holder->indx + 1], 
			ft_strlen(&holder->result[holder->indx + 1]) + 1);
	return (1);
}

char	*expand_variables(char *str, t_shell *shell)
{
	t_expand	holder;
	char		*tilde_expanded;
	int			expansion_result;

	if (!str)
		return (NULL);
	tilde_expanded = expand_tilde(str, shell);
	if (!tilde_expanded)
		return (NULL);
	
	// Initialize all fields of holder to prevent uninitialized access
	holder.result = tilde_expanded;
	holder.var_start = NULL;
	holder.var_end = NULL;
	holder.var_name = NULL;
	holder.var_value = NULL;
	holder.expanded = NULL;
	holder.indx = 0;
	holder.var_len = 0;
	
	while (holder.result[holder.indx])
	{
		if (holder.result[holder.indx] == '$')
		{
			expansion_result = handle_dollar_expansion(&holder, shell);
			if (expansion_result == 0)
				break ;
			if (expansion_result == 1)
				continue ;
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
	result = expand_with_quotes(tilde_expanded, shell);
	// Always return result, freeing tilde_expanded only if it's different
	// expand_with_quotes either returns the same pointer or a new allocation
	if (result != tilde_expanded && tilde_expanded)
		free(tilde_expanded);
	return (result);
}

char	*expand_with_quotes(char *str, t_shell *shell)
{
	t_expand	holder;
	char		*tilde_expanded;
	int			expansion_result;
	int			in_single_quotes;
	int			in_double_quotes;

	if (!str)
		return (NULL);
	tilde_expanded = expand_tilde(str, shell);
	if (!tilde_expanded)
		return (NULL);
	
	// Initialize all fields of holder
	holder.result = tilde_expanded;
	holder.var_start = NULL;
	holder.var_end = NULL;
	holder.var_name = NULL;
	holder.var_value = NULL;
	holder.expanded = NULL;
	holder.indx = 0;
	holder.var_len = 0;
	in_single_quotes = 0;
	in_double_quotes = 0;
	
	while (holder.result[holder.indx])
	{
		// Track quote state but don't skip the quote characters
		if (holder.result[holder.indx] == '\'' && !in_double_quotes)
			in_single_quotes = !in_single_quotes;
		else if (holder.result[holder.indx] == '"' && !in_single_quotes)
			in_double_quotes = !in_double_quotes;
		// Only expand $ if not in single quotes
		else if (holder.result[holder.indx] == '$' && !in_single_quotes)
		{
			expansion_result = handle_dollar_expansion(&holder, shell);
			if (expansion_result == 0)
				break ;
			if (expansion_result == 1)
				continue ;
		}
		holder.indx++;
	}
	return (holder.result);
}
