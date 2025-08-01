/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_if_path_is_dir.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 15:32:39 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 17:15:17 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	handle_question_mark(t_expand *holder, t_shell *shell)
{
	holder->var_value = ft_itoa(shell->past_exit_status);
	if (!holder->var_value)
		return (0);
	if (!replacevar(holder, 3))
		return (0);
	return (1);
}

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
