/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 20:59:39 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	is_special_directory(char *cmd)
{
	if ((ft_strncmp(cmd, "..", 3) == 0 && ft_strlen(cmd) == 2)
		|| (ft_strncmp(cmd, ".", 2) == 0 && ft_strlen(cmd) == 1))
		return (1);
	return (0);
}

static char	*check_absolute_path(char *cmd)
{
	struct stat	file_stat;

	if (ft_strchr(cmd, '/'))
	{
		if (not_dir(cmd))
			return (ft_strdup("__NOT_DIRECTORY__"));
		if (access(cmd, F_OK) == 0)
		{
			if (stat(cmd, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
				return (ft_strdup("__IS_DIRECTORY__"));
			if (access(cmd, X_OK) != 0)
				return (ft_strdup("__NOT_EXECUTABLE__"));
			return (ft_strdup(cmd));
		}
		return (ft_strdup("__NOT_FOUND__"));
	}
	if (access(cmd, F_OK) == 0)
	{
		if (stat(cmd, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
			if (is_special_directory(cmd))
				return (NULL);
		return (ft_strdup(cmd));
	}
	return (NULL);
}

static char	*search_in_path(char *cmd, char **paths)
{
	char	*temp;
	char	*full_path;
	int		i;

	i = 0;
	while (paths[i])
	{
		temp = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(temp, cmd);
		free(temp);
		if (full_path && access(full_path, F_OK) == 0)
		{
			free_args(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	free_args(paths);
	return (NULL);
}

char	*find_executable(char *cmd, t_shell *shell)
{
	char	*path_env;
	char	**paths;
	char	*result;

	result = check_absolute_path(cmd);
	result = handle_absolute_path_result(result, cmd);
	if (result)
	{
		if (ft_strncmp(result, "__NOT_DIRECTORY__", 17) == 0)
		{
			free(result);
			return (ft_strdup("__NOT_DIRECTORY__"));
		}
		return (result);
	}
	if (ft_strchr(cmd, '/'))
		return (NULL);
	path_env = get_env_value("PATH", shell);
	if (!path_env)
		return (handle_no_path_env(cmd));
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	return (search_in_path(cmd, paths));
}
