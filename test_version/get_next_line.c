/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tester_version <tester_version@42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 00:00:00 by tester_version   #+#    #+#             */
/*   Updated: 2025/07/21 00:00:00 by tester_version  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main/minishell.h"

#define BUFFER_SIZE 1024

static char	*ft_strjoin_gnl(char *s1, char *s2)
{
	char	*result;
	int		i;
	int		j;

	if (!s1)
		s1 = ft_strdup("");
	if (!s1 || !s2)
		return (NULL);
	result = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!result)
		return (NULL);
	i = 0;
	while (s1[i])
	{
		result[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2[j])
	{
		result[i + j] = s2[j];
		j++;
	}
	result[i + j] = '\0';
	free(s1);
	return (result);
}

static char	*extract_line(char *buffer)
{
	char	*line;
	int		i;

	if (!buffer || !buffer[0])
		return (NULL);
	i = 0;
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (buffer[i] == '\n')
		i++;
	line = malloc(sizeof(char) * (i + 1));
	if (!line)
		return (NULL);
	i = 0;
	while (buffer[i] && buffer[i] != '\n')
	{
		line[i] = buffer[i];
		i++;
	}
	if (buffer[i] == '\n')
		line[i++] = '\n';
	line[i] = '\0';
	return (line);
}

static char	*update_buffer(char *buffer)
{
	char	*new_buffer;
	int		i;
	int		j;

	i = 0;
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (!buffer[i])
	{
		free(buffer);
		return (NULL);
	}
	i++;
	new_buffer = malloc(sizeof(char) * (ft_strlen(buffer) - i + 1));
	if (!new_buffer)
		return (NULL);
	j = 0;
	while (buffer[i + j])
	{
		new_buffer[j] = buffer[i + j];
		j++;
	}
	new_buffer[j] = '\0';
	free(buffer);
	return (new_buffer);
}

char	*get_next_line(int fd)
{
	static char	*buffer;
	char		*read_buffer;
	char		*line;
	int			bytes_read;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	read_buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!read_buffer)
		return (NULL);
	bytes_read = 1;
	while (bytes_read > 0 && (!buffer || !ft_strchr(buffer, '\n')))
	{
		bytes_read = read(fd, read_buffer, BUFFER_SIZE);
		if (bytes_read == -1)
		{
			free(read_buffer);
			return (NULL);
		}
		read_buffer[bytes_read] = '\0';
		buffer = ft_strjoin_gnl(buffer, read_buffer);
	}
	free(read_buffer);
	line = extract_line(buffer);
	buffer = update_buffer(buffer);
	return (line);
}
