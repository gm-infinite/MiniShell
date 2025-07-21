/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 18:41:56 by emgenc           ###   ########.fr       */
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

	// Commands with '/' are treated as paths and should NEVER fall back to PATH
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, F_OK) == 0)
		{
			// Check if it's a directory
			if (stat(cmd, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
			{
				// For directories, we need to set error appropriately
				// Return a special marker to indicate "is directory" error
				return (ft_strdup("__IS_DIRECTORY__"));
			}
			// Check if it's executable
			if (access(cmd, X_OK) != 0)
			{
				// File exists but not executable - return special marker
				return (ft_strdup("__NOT_EXECUTABLE__"));
			}
			return (ft_strdup(cmd));
		}
		// Path doesn't exist - return special marker
		return (ft_strdup("__NOT_FOUND__"));
	}
	
	// Commands without '/' - check if they exist in current directory
	if (access(cmd, F_OK) == 0)
	{
		if (stat(cmd, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
		{
			if (is_special_directory(cmd))
				return (NULL);  // Let these be handled specially
		}
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

static void	write_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

char	*find_executable(char *cmd, t_shell *shell)
{
	char	*path_env;
	char	**paths;
	char	*result;

	result = check_absolute_path(cmd);
	
	// Handle special error cases for paths with '/'
	if (result && ft_strncmp(result, "__IS_DIRECTORY__", 16) == 0)
	{
		free(result);
		write_error_message(cmd, ": Is a directory\n");
		return (NULL);
	}
	if (result && ft_strncmp(result, "__NOT_EXECUTABLE__", 18) == 0)
	{
		free(result);
		write_error_message(cmd, ": Permission denied\n");
		return (NULL);
	}
	if (result && ft_strncmp(result, "__NOT_FOUND__", 13) == 0)
	{
		free(result);
		write_error_message(cmd, ": No such file or directory\n");
		return (NULL);
	}
	
	if (result)
		return (result);
		
	// Only search PATH if command doesn't contain '/'
	if (ft_strchr(cmd, '/'))
		return (NULL);  // Path commands should never reach here
		
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
