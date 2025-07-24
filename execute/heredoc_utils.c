/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 14:23:10 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static char	*ft_realloc_join(char *s1, char *s2)
{
	char	*result;

	result = ft_strjoin(s1, s2);
	free(s1);
	return (result);
}

char	*create_heredoc_filename(int cmd_index)
{
	char			*result;
	char			*temp_str;
	unsigned long	addr;
	int				local_var;

	addr = (unsigned long)&local_var;
	result = ft_strdup("/tmp/.minishell_heredoc_");
	temp_str = ft_itoa(addr % 1000000);
	result = ft_realloc_join(result, temp_str);
	free(temp_str);
	result = ft_realloc_join(result, "_");
	temp_str = ft_itoa(cmd_index);
	result = ft_realloc_join(result, temp_str);
	free(temp_str);
	return (result);
}

int	read_heredoc_line(char **line)
{
	*line = readline("> ");
	return (*line != NULL);
}

static void	process_and_write_heredoc_line(int temp_fd, char *line,
		t_shell *shell, int should_expand)
{
	char	*expanded_line;

	if (should_expand && shell)
	{
		expanded_line = expand_variables(line, shell);
		if (expanded_line)
		{
			write(temp_fd, expanded_line, ft_strlen(expanded_line));
			free(expanded_line);
		}
		else
			write(temp_fd, line, ft_strlen(line));
	}
	else
		write(temp_fd, line, ft_strlen(line));
	write(temp_fd, "\n", 1);
}

int	process_heredoc_content(int temp_fd, char *processed_delimiter,
		t_shell *shell, int should_expand)
{
	char	*line;
	size_t	delimiter_len;

	delimiter_len = ft_strlen(processed_delimiter);
	while (1)
	{
		if (!read_heredoc_line(&line))
		{
			write_heredoc_warning(processed_delimiter);
			break ;
		}
		if (ft_strncmp(line, processed_delimiter, delimiter_len) == 0
			&& ft_strlen(line) == delimiter_len)
		{
			free(line);
			break ;
		}
		process_and_write_heredoc_line(temp_fd, line, shell, should_expand);
		free(line);
	}
	return (0);
}
