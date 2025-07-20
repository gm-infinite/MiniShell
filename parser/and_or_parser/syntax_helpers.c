/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 00:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/21 00:36:38 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	check_empty_parentheses(t_split split)
{
	int	i;

	i = -1;
	while (++i < split.size)
	{
		if (ft_strnstr(split.start[i], "()", ft_strlen(split.start[i])))
			return (write(STDERR_FILENO,
					"bash: syntax error near unexpected token `)'\n", 45), 0);
		if (i < split.size - 1)
			if (ft_strncmp(split.start[i], "(", 2) == 0
				&& ft_strncmp(split.start[i + 1], ")", 2) == 0)
				return (write(STDERR_FILENO,
						"bash: syntax error near unexpected token `)'\n",
						45), 0);
	}
	return (1);
}

int	check_parentheses_adjacency(t_split split)
{
	int	i;

	i = -1;
	while (++i < split.size - 1)
	{
		if (ft_strchr(split.start[i], ')')
			&& ft_strchr(split.start[i + 1], '('))
			return (write(STDERR_FILENO,
					"bash: syntax error near unexpected token `('\n", 45), 0);
		if (ft_strchr(split.start[i], ')') && split.start[i + 1]
			&& !ft_strchr(split.start[i + 1], '(')
			&& ft_strncmp(split.start[i + 1], "&&", 3) != 0
			&& ft_strncmp(split.start[i + 1], "||", 3) != 0
			&& ft_strncmp(split.start[i + 1], "|", 2) != 0)
			return (print_syntax_error(split.start[i + 1]), 0);
	}
	return (1);
}
