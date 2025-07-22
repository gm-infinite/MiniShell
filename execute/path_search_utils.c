/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_search_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 18:41:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static void	write_path_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

char	*handle_absolute_path_result(char *result, char *cmd)
{
	if (!result)
		return (NULL);
	if (ft_strncmp(result, "__IS_DIRECTORY__", 16) == 0)
	{
		free(result);
		write_path_error_message(cmd, ": Is a directory\n");
		return (NULL);
	}
	if (ft_strncmp(result, "__NOT_EXECUTABLE__", 18) == 0)
	{
		free(result);
		write_path_error_message(cmd, ": Permission denied\n");
		return (NULL);
	}
	if (ft_strncmp(result, "__NOT_FOUND__", 13) == 0)
	{
		free(result);
		write_path_error_message(cmd, ": No such file or directory\n");
		return (NULL);
	}
	return (result);
}

char	*handle_no_path_env(char *cmd)
{
	if (access(cmd, F_OK) == 0)
		return (ft_strdup(cmd));
	return (NULL);
}
