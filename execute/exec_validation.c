/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_validation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:10:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 20:40:33 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	validate_and_process_args(char **args, t_shell *shell)
{
	int		i;
	char	*expanded;

	if (!args || !args[0])
		return (1);
	i = 0;
	while (args[i])
	{
		expanded = expand_variables_quoted(args[i], shell);
		if (expanded && expanded != args[i])
		{
			free(args[i]);
			args[i] = expanded;
		}
		i++;
	}
	process_args_quotes(args, shell);
	if (!args[0])
		return (0);
	if (args[0][0] == '\0')
	{
		write(STDERR_FILENO, ": command not found\n", 20);
		return (127);
	}
	return (-1);
}

static void	write_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

int	validate_executable(char *cmd, char *executable)
{
	struct stat	file_stat;

	if (stat(executable, &file_stat) == 0)
	{
		if (S_ISDIR(file_stat.st_mode))
		{
			if ((ft_strncmp(cmd, ".", 2) == 0 && ft_strlen(cmd) == 1)
				|| (ft_strncmp(cmd, "..", 3) == 0 && ft_strlen(cmd) == 2))
			{
				write_error_message(cmd, ": command not found\n");
				return (127);
			}
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

int	handle_executable_not_found(char **args)
{
	struct stat	file_stat;

	if ((ft_strncmp(args[0], ".", 2) == 0 && ft_strlen(args[0]) == 1)
		|| (ft_strncmp(args[0], "..", 3) == 0 && ft_strlen(args[0]) == 2))
	{
		write_error_message(args[0], ": command not found\n");
		return (127);
	}
	if (ft_strchr(args[0], '/'))
	{
		if (has_not_directory_error(args[0]))
		{
			write_not_directory_error(args[0]);
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
