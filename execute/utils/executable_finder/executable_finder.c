/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executable_finder.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 11:49:58 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 23:47:13 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../main/minishell.h"

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

static char	*handle_err(char *result, char *cmd, char *token, char *message)
{
	if (ft_strncmp(result, token, ft_strlen(token)) == 0)
	{
		free(result);
		if (message)
			write_path_error_message(cmd, message);
		if (ft_strncmp(token, "__NOT_DIRECTORY__", 17) == 0)
			return (ft_strdup("__NOT_DIRECTORY__"));
		return (NULL);
	}
	return (result);
}

static char	*handle_absolute_path_result(char *result, char *cmd)
{
	char	*temp;

	if (!result)
		return (NULL);
	temp = handle_err(result, cmd, "__IS_DIRECTORY__",
			": Is a directory\n");
	if (temp != result)
		return (temp);
	temp = handle_err(result, cmd, "__NOT_DIRECTORY__", NULL);
	if (temp != result)
		return (temp);
	temp = handle_err(result, cmd,
			"__NOT_EXECUTABLE__", ": Permission denied\n");
	if (temp != result)
		return (temp);
	temp = handle_err(result, cmd,
			"__NOT_FOUND__", ": No such file or directory\n");
	if (temp != result)
		return (temp);
	return (result);
}

char	*find_executable(char *cmd, t_shell *shell)
{
	char	*path_env;
	char	**paths;
	char	*result;

	result = check_absolute_path(cmd);
	result = handle_absolute_path_result(result, cmd);
	if (result)
		return (result);
	if (ft_strchr(cmd, '/'))
		return (NULL);
	path_env = get_env_value("PATH", shell);
	if (!path_env)
	{
		if (access(cmd, F_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	return (search_in_path(cmd, paths));
}
