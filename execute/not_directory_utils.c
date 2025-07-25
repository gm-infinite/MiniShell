/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   not_directory_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 00:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 20:40:33 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	check_path_component_is_directory(char *path, int len)
{
	char		*component_path;
	struct stat	file_stat;
	int			result;

	component_path = malloc(len + 1);
	if (!component_path)
		return (0);
	ft_strlcpy(component_path, path, len + 1);
	if (access(component_path, F_OK) != 0)
	{
		free(component_path);
		return (0);
	}
	result = 0;
	if (stat(component_path, &file_stat) == 0)
	{
		if (!S_ISDIR(file_stat.st_mode))
			result = 1;
	}
	free(component_path);
	return (result);
}

int	not_dir(char *path)
{
	char	*ptr;
	int		len;

	if (!path || !ft_strchr(path, '/'))
		return (0);
	ptr = path;
	while (*ptr)
	{
		if (*ptr == '/')
		{
			len = ptr - path;
			if (len > 0 && check_path_component_is_directory(path, len))
				return (1);
		}
		ptr++;
	}
	return (0);
}

void	write_notdir(char *filename)
{
	write(STDERR_FILENO, filename, ft_strlen(filename));
	write(STDERR_FILENO, ": Not a directory\n", 18);
}
