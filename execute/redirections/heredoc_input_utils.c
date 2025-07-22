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
#include "../../main/get_next_line.h"

char	*read_heredoc_input_line(void)
{
	char	*line;
	char	*trimmed;

	if (isatty(fileno(stdin)))
		line = readline("> ");
	else
	{
		line = get_next_line(fileno(stdin));
		if (line)
		{
			trimmed = ft_strtrim(line, "\n");
			free(line);
			line = trimmed;
		}
	}
	return (line);
}

void	write_heredoc_warning_message(char *delimiter)
{
	write(STDERR_FILENO,
		"minishell: warning: here-document delimited by end-of-file (wanted `",
		68);
	write(STDERR_FILENO, delimiter, ft_strlen(delimiter));
	write(STDERR_FILENO, "')\n", 3);
}

int	is_delimiter_match(char *line, char *delimiter)
{
	if (ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0
		&& ft_strlen(line) == ft_strlen(delimiter))
		return (1);
	return (0);
}
