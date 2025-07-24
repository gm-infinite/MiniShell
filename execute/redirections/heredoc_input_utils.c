/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_input_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:53:10 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:53:36 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

char	*read_heredoc_input_line(void)
{
	char	*line;

	line = readline("> ");
	return (line);
}

int	is_delimiter_match(char *line, char *delimiter)
{
	if (ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0
		&& ft_strlen(line) == ft_strlen(delimiter))
		return (1);
	return (0);
}
