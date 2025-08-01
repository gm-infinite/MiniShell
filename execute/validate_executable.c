/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_executable.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:40:21 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 01:15:43 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static void	write_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

static int	special_case(char *cmd)
{
	if (ft_strncmp(cmd, ".", 2) == 0 && ft_strlen(cmd) == 1)
	{
		write_error_message(cmd, ": filename argument required\n");
		return (2);
	}
	if (ft_strncmp(cmd, "..", 3) == 0 && ft_strlen(cmd) == 2)
	{
		write_error_message(cmd, ": command not found\n");
		return (127);
	}
	return (0);
}

static int	handle_executable_not_found(char **args)
{
	struct stat	file_stat;

	if ((ft_strncmp(args[0], "..", 3) == 0 && ft_strlen(args[0]) == 2)
		|| (ft_strncmp(args[0], ".", 2) == 0 && ft_strlen(args[0]) == 1))
		return (special_case(args[0]));
	if (ft_strchr(args[0], '/'))
	{
		if (not_dir(args[0]))
		{
			write_notdir(args[0]);
			return (126);
		}
		if (access(args[0], F_OK) == 0 && stat(args[0], &file_stat) == 0
			&& S_ISDIR(file_stat.st_mode))
			return (126);
		return (127);
	}
	else
	{
		write_error_message(args[0], ": command not found\n");
		return (127);
	}
}

int	validate_executable(char *cmd, char *executable)
{
	struct stat	file_stat;

	if (stat(executable, &file_stat) == 0)
	{
		if (S_ISREG(file_stat.st_mode) && access(executable, X_OK) != 0)
		{
			write_error_message(cmd, ": command not found\n");
			return (127);
		}
		if (S_ISDIR(file_stat.st_mode))
		{
			if ((ft_strncmp(cmd, "..", 3) == 0 && ft_strlen(cmd) == 2)
				|| (ft_strncmp(cmd, ".", 2) == 0 && ft_strlen(cmd) == 1))
				return (special_case(cmd));
			write_error_message(cmd, ": Is a directory\n");
			return (126);
		}
		if (access(executable, X_OK) != 0)
		{
			write_error_message(cmd, ": Permission denied\n");
			return (126);
		}
	}
	return (0);
}

int	validate_exec(char **args, char *executable)
{
	if (!executable)
		return (handle_executable_not_found(args));
	if (ft_strncmp(executable, "__NOT_DIRECTORY__", 17) == 0)
	{
		write_notdir(args[0]);
		free(executable);
		return (126);
	}
	return (validate_executable(args[0], executable));
}
