/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_syntax_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 22:45:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/21 00:38:17 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

void	print_pipe_error(void)
{
	write(STDERR_FILENO,
		"bash: syntax error near unexpected token `|'\n", 45);
}

void	print_newline_error(void)
{
	write(STDERR_FILENO,
		"bash: syntax error near unexpected token `newline'\n", 52);
}

void	print_token_error(char *token)
{
	write(STDERR_FILENO, "bash: syntax error near unexpected token `", 42);
	write(STDERR_FILENO, token, ft_strlen(token));
	write(STDERR_FILENO, "'\n", 2);
}

int	check_pipe_error(t_split split, int i, int has_parentheses)
{
	if (!has_parentheses && (i == 0 || i == split.size - 1))
	{
		print_pipe_error();
		return (2);
	}
	return (0);
}

int	check_redirection_error(t_split split, int i)
{
	int		filename_index;
	char	*next_token;

	filename_index = i + 1;
	if (filename_index >= split.size)
	{
		print_newline_error();
		return (2);
	}
	next_token = split.start[filename_index];
	if (is_redirection(next_token) || ft_strncmp(next_token, "|", 2) == 0)
	{
		print_token_error(next_token);
		return (2);
	}
	return (0);
}
