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
		if (holder.result[holder.indx] == '\'' && !in_double_quotes)
			in_single_quotes = !in_single_quotes;
		else if (holder.result[holder.indx] == '"' && !in_single_quotes)
			in_double_quotes = !in_double_quotes;
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

int	handle_double_quote_content(t_expand *holder, int i, char *content)
{
	char	*new_str;
	char	*escaped_content;
	int		result;

	result = create_escaped_content(content, &escaped_content);
	if (!result)
		escaped_content = content;
	new_str = ft_calloc(ft_strlen(holder->result)
			+ ft_strlen(escaped_content) + 1, 1);
	if (new_str)
	{
		ft_strlcpy(new_str, holder->result, holder->indx + 1);
		ft_strlcat(new_str, escaped_content, ft_strlen(holder->result)
			+ ft_strlen(escaped_content) + 1);
		ft_strlcat(new_str, &holder->result[i + 1], ft_strlen(holder->result)
			+ ft_strlen(escaped_content) + 1);
		free(holder->result);
		holder->result = new_str;
		holder->indx += ft_strlen(escaped_content);
	}
	if (escaped_content != content)
		free(escaped_content);
	return (1);
}

int	create_escaped_content(char *content, char **escaped_content)
{
	int	j;
	int	k;

	j = 0;
	k = 0;
	*escaped_content = malloc(ft_strlen(content) * 2 + 1);
	if (!*escaped_content)
		return (0);
	while (content[j])
	{
		if (content[j] == '\'')
		{
			(*escaped_content)[k++] = '\001';
			(*escaped_content)[k++] = '\'';
		}
		else
			(*escaped_content)[k++] = content[j];
		j++;
	}
	(*escaped_content)[k] = '\0';
	return (1);
}

int	handle_double_quote_expansion(t_expand *holder)
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

int	handle_single_quote_expansion(t_expand *holder)
{
	int		i;
	char	*content;
	char	*new_str;

	i = holder->indx + 2;
	while (holder->result[i] && holder->result[i] != '\'')
		i++;
	if (holder->result[i] == '\'')
	{
		content = ft_substr(holder->result, holder->indx + 2,
				i - holder->indx - 2);
		if (content)
		{
			new_str = ft_calloc(ft_strlen(holder->result) + 1, 1);
			if (new_str)
			{
				ft_strlcpy(new_str, holder->result, holder->indx + 1);
				ft_strlcat(new_str, content, ft_strlen(holder->result) + 1);
				ft_strlcat(new_str, &holder->result[i + 1],
					ft_strlen(holder->result) + 1);
				free(holder->result);
				holder->result = new_str;
				holder->indx += ft_strlen(content);
			}
			free(content);
			return (1);
		}
	}
	holder->indx++;
	return (1);
}

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