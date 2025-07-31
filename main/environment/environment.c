/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:10:45 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 17:03:50 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

int	process_single_quote_content(t_expand *holder, int i, char *content)
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

char	*get_env_value(char *var_name, t_shell *shell)
{
	int		i;
	int		len;
	char	*eq_pos;

	if (!var_name || !shell->envp)
		return (NULL);
	len = ft_strlen(var_name);
	i = 0;
	while (shell->envp[i])
	{
		eq_pos = ft_strchr(shell->envp[i], '=');
		if (eq_pos && (eq_pos - shell->envp[i]) == len)
			if (ft_strncmp(shell->envp[i], var_name, len) == 0)
				return (eq_pos + 1);
		i++;
	}
	return (NULL);
}
