/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 18:54:11 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/31 18:54:49 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../main/minishell.h"

void	process_and_write_line(char *line, int pipe_fd, t_shell *shell,
		int should_expand)
{
	char	*expanded_line;

	if (should_expand && shell)
	{
		expanded_line = expandvar(line, shell);
		if (expanded_line)
		{
			write(pipe_fd, expanded_line, ft_strlen(expanded_line));
			write(pipe_fd, "\n", 1);
			free(expanded_line);
		}
		else
		{
			write(pipe_fd, line, ft_strlen(line));
			write(pipe_fd, "\n", 1);
		}
	}
	else
	{
		write(pipe_fd, line, ft_strlen(line));
		write(pipe_fd, "\n", 1);
	}
}

int	delimiter_was_quoted(char *filename)
{
	int	i;

	if (!filename)
		return (0);
	i = 0;
	while (filename[i])
	{
		if (filename[i] == '\'' || filename[i] == '"')
			return (1);
		i++;
	}
	return (0);
}
