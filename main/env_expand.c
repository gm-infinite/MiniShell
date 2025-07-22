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
	{
		// Lone $ at end of string - keep it
		holder->indx++;
		return (1);
	}
	holder->var_start = &holder->result[holder->indx + 1];
	
	// Handle $"string" locale translation syntax
	if (*holder->var_start == '"')
	{
		// For $"...", we need to find the closing quote and treat content as literal
		int		i;
		char	*content;
		char	*new_str;
		char	*escaped_content;
		
		i = holder->indx + 2;
		while (holder->result[i] && holder->result[i] != '"')
			i++;
		// Check if we found a closing quote and there's actually content between quotes
		if (holder->result[i] == '"' && i > holder->indx + 2)
		{
			// Extract content between quotes
			content = ft_substr(holder->result, holder->indx + 2, i - holder->indx - 2);
			if (content)
			{
				// Escape single quotes to protect them from quote removal
				escaped_content = malloc(ft_strlen(content) * 2 + 1);
				if (escaped_content)
				{
					int j = 0, k = 0;
					while (content[j])
					{
						if (content[j] == '\'')
						{
							escaped_content[k++] = '\001'; // Use SOH as escape marker
							escaped_content[k++] = '\'';
						}
						else
							escaped_content[k++] = content[j];
						j++;
					}
					escaped_content[k] = '\0';
				}
				else
					escaped_content = content;
				
				// Create new string without $"..."
				new_str = ft_calloc(ft_strlen(holder->result) + ft_strlen(escaped_content) + 1, 1);
				if (new_str)
				{
					ft_strlcpy(new_str, holder->result, holder->indx + 1);
					ft_strlcat(new_str, escaped_content, ft_strlen(holder->result) + ft_strlen(escaped_content) + 1);
					ft_strlcat(new_str, &holder->result[i + 1], ft_strlen(holder->result) + ft_strlen(escaped_content) + 1);
					free(holder->result);
					holder->result = new_str;
					holder->indx += ft_strlen(escaped_content);
				}
				if (escaped_content != content)
					free(escaped_content);
				free(content);
				return (1);
			}
		}
		// If no closing quote found, or quote is immediately after $", or empty content
		// This is just a lone $ followed by a quote character - keep it literal
		holder->indx++;
		return (1);
	}
	
	// Handle $'string' ANSI-C quoting
	if (*holder->var_start == '\'')
	{
		// For $'...', we need to find the closing quote and treat content as literal
		int		i;
		char	*content;
		char	*new_str;
		
		i = holder->indx + 2;
		while (holder->result[i] && holder->result[i] != '\'')
			i++;
		if (holder->result[i] == '\'')
		{
			// Extract content between quotes
			content = ft_substr(holder->result, holder->indx + 2, i - holder->indx - 2);
			if (content)
			{
				// Create new string without $'...'
				new_str = ft_calloc(ft_strlen(holder->result) + 1, 1);
				if (new_str)
				{
					ft_strlcpy(new_str, holder->result, holder->indx + 1);
					ft_strlcat(new_str, content, ft_strlen(holder->result) + 1);
					ft_strlcat(new_str, &holder->result[i + 1], ft_strlen(holder->result) + 1);
					free(holder->result);
					holder->result = new_str;
					holder->indx += ft_strlen(content);
				}
				free(content);
				return (1);
			}
		}
		// If no closing quote or error, keep the $
		holder->indx++;
		return (1);
	}
	
	// Check if next char is space or special character that means $ should be kept
	if (*holder->var_start == ' ' || *holder->var_start == '/' 
		|| *holder->var_start == '\t' || *holder->var_start == '\n')
	{
		// Keep the $ literal
		holder->indx++;
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
	// Lone $ followed by quote character or other special char - keep it literal
	// This fixes cases like "$" where $ is followed by closing quote
	if (*holder->var_start == '"' || *holder->var_start == '\'' || 
		*holder->var_start == '$' || !ft_isprint(*holder->var_start))
	{
		holder->indx++;
		return (1);
	}
	// Lone $ not followed by valid variable name - keep it literal (changed from remove)
	// This matches bash behavior where lone $ is preserved
	holder->indx++;
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
