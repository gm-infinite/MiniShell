/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_quote_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 14:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 18:41:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	create_escaped_content(char *content, char **escaped_content)
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

static int	handle_double_quote_content(t_expand *holder, int i, char *content)
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

static int	process_single_quote_content(t_expand *holder, int i, char *content)
{
	char	*new_str;

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
		return (1);
	}
	return (0);
}

int	handle_single_quote_expansion(t_expand *holder)
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
