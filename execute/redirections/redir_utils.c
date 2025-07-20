/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:52:11 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 12:33:49 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

char	*remove_quotes_for_redirection(char *str)
{
	char			*result;
	int				i;
	int				j;
	unsigned char	quote_state;
	int				len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	result = malloc(len + 1);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	quote_state = 0;
	while (str[i])
	{
		if (str[i] == '\'' && !(quote_state & 2))
			quote_state ^= 1;
		else if (str[i] == '"' && !(quote_state & 1))
			quote_state ^= 2;
		else
		{
			result[j] = str[i];
			j++;
		}
		i++;
	}
	result[j] = '\0';
	return (result);
}

int	is_redirection(char *token)
{
	if (!token)
		return (0);
	if (ft_strncmp(token, "<<", 3) == 0 && ft_strlen(token) == 2)
		return (1);
	if (ft_strncmp(token, ">>", 3) == 0 && ft_strlen(token) == 2)
		return (2);
	if (ft_strncmp(token, "<", 2) == 0 && ft_strlen(token) == 1)
		return (3);
	if (ft_strncmp(token, ">", 2) == 0 && ft_strlen(token) == 1)
		return (4);
	return (0);
}


int	validate_redirection_syntax(t_split split)
{
	int	i;
	int	redirect_type;
	int	has_parentheses = 0;

	i = -1;
	while (++i < split.size)
	{
		if (ft_strchr(split.start[i], '(') || ft_strchr(split.start[i], ')'))
		{
			has_parentheses = 1;
			break;
		}
	}
	i = 0;
	while (i < split.size)
	{
		redirect_type = is_redirection(split.start[i]);

		if (redirect_type)
		{
			int filename_index = i + 1;
			if (filename_index >= split.size)
			{
				write(STDERR_FILENO, "bash: syntax error near unexpected token `newline'\n", 52);
				return (2);
			}
			char *next_token = split.start[filename_index];
			if (is_redirection(next_token) || ft_strncmp(next_token, "|", 2) == 0)
			{
				write(STDERR_FILENO, "bash: syntax error near unexpected token `", 42);
				write(STDERR_FILENO, next_token, ft_strlen(next_token));
				write(STDERR_FILENO, "'\n", 2);
				return (2);
			}
			i = filename_index + 1;
		}
		else if (ft_strncmp(split.start[i], "|", 2) == 0 && ft_strlen(split.start[i]) == 1)
		{
			if (!has_parentheses)
			{
				if (i == 0 || i == split.size - 1)
				{
					write(STDERR_FILENO, "bash: syntax error near unexpected token `|'\n", 45);
					return (2);
				}
			}
			i++;
		}
		else
			i++;
	}
	return (0);
}
