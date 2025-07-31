/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_syntax_validation.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:37:52 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 20:01:38 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static int	handle_pipe_token(t_split split, int i, int has_parentheses)
{
	if (ft_strncmp(split.start[i], "|", 2) == 0
		&& ft_strlen(split.start[i]) == 1)
	{
		if (!has_parentheses && (i == 0 || i == split.size - 1))
		{
			write(STDERR_FILENO,
				"bash: syntax error near unexpected token `|'\n", 45);
			return (2);
		}
	}
	return (0);
}

static int	check_redirection_error(t_split split, int i)
{
	int		filename_index;
	char	*next_token;

	filename_index = i + 1;
	if (filename_index >= split.size)
	{
		write(STDERR_FILENO,
			"bash: syntax error near unexpected token `newline'\n", 52);
		return (2);
	}
	next_token = split.start[filename_index];
	if (is_redirection(next_token) || ft_strncmp(next_token, "|", 2) == 0)
	{
		write(STDERR_FILENO, "bash: syntax error near unexpected token `", 42);
		write(STDERR_FILENO, next_token, ft_strlen(next_token));
		write(STDERR_FILENO, "'\n", 2);
		return (2);
	}
	return (0);
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

static int	has_parentheses_in_split(t_split split)
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
