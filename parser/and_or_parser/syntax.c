/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 23:29:57 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 22:00:13 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	print_syntax_error(char *token)
{
	write(STDERR_FILENO, "bash: syntax error near unexpected token `", 42);
	write(STDERR_FILENO, token, ft_strlen(token));
	write(STDERR_FILENO, "'\n", 2);
}

int	check_operator_syntax_errors(t_split split)
{
	int		i;
	char	*last_token;

	if (split.size > 0)
	{
		last_token = split.start[split.size - 1];
		if (ft_strncmp(last_token, "&&", 3) == 0
			|| ft_strncmp(last_token, "||", 3) == 0)
			return (write(STDERR_FILENO,
					"bash: syntax error: unexpected end of file2\n", 44), 0);
	}
	i = -1;
	while (++i < split.size - 1)
	{
		if ((ft_strncmp(split.start[i], "&&", 3) == 0
				|| ft_strncmp(split.start[i], "||", 3) == 0)
			&& (ft_strncmp(split.start[i + 1], "&&", 3) == 0
				|| ft_strncmp(split.start[i + 1], "||", 3) == 0))
			return (print_syntax_error(split.start[i + 1]), 0);
	}
	return (1);
}

int	check_parentheses_syntax_errors(t_split split)
{
	int	i;

	i = -1;
	while (++i < split.size)
	{
		if (ft_strnstr(split.start[i], "()", ft_strlen(split.start[i])))
			return (write(STDERR_FILENO,
					"bash: syntax error near unexpected token `)'\n", 45), 0);
		if (i < split.size - 1 && ft_strncmp(split.start[i], "(", 2) == 0
			&& ft_strncmp(split.start[i + 1], ")", 2) == 0)
			return (write(STDERR_FILENO,
					"bash: syntax error near unexpected token `)'\n", 45), 0);
		if (i < split.size - 1 && ft_strchr(split.start[i], ')')
			&& ft_strchr(split.start[i + 1], '('))
			return (write(STDERR_FILENO,
					"bash: syntax error near unexpected token `('\n", 45), 0);
		if (i < split.size - 1 && ft_strchr(split.start[i], ')')
			&& split.start[i + 1] && !ft_strchr(split.start[i + 1], '(')
			&& ft_strncmp(split.start[i + 1], "&&", 3) != 0
			&& ft_strncmp(split.start[i + 1], "||", 3) != 0
			&& ft_strncmp(split.start[i + 1], "|", 2) != 0)
			return (print_syntax_error(split.start[i + 1]), 0);
	}
	return (1);
}
