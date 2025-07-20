/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_syntax.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 12:53:07 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 22:45:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	has_parentheses_in_split(t_split split)
{
	int	i;

	i = 0;
	while (i < split.size)
	{
		if (ft_strchr(split.start[i], '(') || ft_strchr(split.start[i], ')'))
			return (1);
		i++;
	}
	return (0);
}

char	*remove_quotes_for_redirection(char *str)
{
	char			*result;
	int				i;
	int				j;
	unsigned char	quote_state;

	if (!str)
		return (NULL);
	result = malloc(ft_strlen(str) + 1);
	if (!result)
		return (NULL);
	i = -1;
	j = -1;
	quote_state = 0;
	while (str[++i])
	{
		if (str[i] == '\'' && !(quote_state & 2))
			quote_state ^= 1;
		else if (str[i] == '"' && !(quote_state & 1))
			quote_state ^= 2;
		else
			result[++j] = str[i];
	}
	result[++j] = '\0';
	return (result);
}

static int	handle_redirection_token(t_split split, int *i)
{
	int	err;

	err = check_redirection_error(split, *i);
	if (err)
		return (err);
	*i += 1;
	return (0);
}

static int	handle_pipe_token(t_split split, int i, int has_parentheses)
{
	int	err;

	if (ft_strncmp(split.start[i], "|", 2) == 0
		&& ft_strlen(split.start[i]) == 1)
	{
		err = check_pipe_error(split, i, has_parentheses);
		if (err)
			return (err);
	}
	return (0);
}

int	validate_redirection_syntax(t_split split)
{
	int	i;
	int	has_parentheses;
	int	err;
	int	redirect_type;

	i = -1;
	has_parentheses = has_parentheses_in_split(split);
	while (++i < split.size)
	{
		redirect_type = is_redirection(split.start[i]);
		if (redirect_type)
		{
			err = handle_redirection_token(split, &i);
			if (err)
				return (err);
		}
		err = handle_pipe_token(split, i, has_parentheses);
		if (err)
			return (err);
	}
	return (0);
}
