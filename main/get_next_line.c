/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 10:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/01/13 10:00:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include "../e-libft/libft.h"

static char	*ft_gnl_strchr(const char *s, int c)
{
	if (!s)
		return (NULL);
	while (*s)
	{
		if (*s == (char)c)
			return ((char *)s);
		s++;
	}
	if ((char)c == '\0')
		return ((char *)s);
	return (NULL);
}

static char	*ft_gnl_strjoin(char const *s1, char const *s2)
{
	char	*result;
	size_t	i;
	size_t	j;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (ft_strdup(s1));
	result = malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
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
		result[i++] = s2[j++];
	result[i] = '\0';
	return (result);
}

static char	*extract_line(char **saved)
{
	char	*line;
	char	*newline_pos;
	char	*temp;
	int		line_len;

	if (!*saved || !**saved)
		return (NULL);
	newline_pos = ft_gnl_strchr(*saved, '\n');
	if (newline_pos)
	{
		line_len = newline_pos - *saved + 1;
		line = ft_substr(*saved, 0, line_len);
		temp = ft_strdup(newline_pos + 1);
		free(*saved);
		*saved = temp;
		return (line);
	}
	line = ft_strdup(*saved);
	free(*saved);
	*saved = NULL;
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*saved = NULL;
	char		*buffer;
	char		*temp;
	int			bytes_read;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	buffer = malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	bytes_read = read(fd, buffer, BUFFER_SIZE);
	while (bytes_read > 0)
	{
		buffer[bytes_read] = '\0';
		temp = ft_gnl_strjoin(saved, buffer);
		free(saved);
		saved = temp;
		if (ft_gnl_strchr(saved, '\n'))
			break ;
		bytes_read = read(fd, buffer, BUFFER_SIZE);
	}
	free(buffer);
	if (bytes_read < 0 || (!saved && bytes_read == 0))
		return (NULL);
	return (extract_line(&saved));
} 