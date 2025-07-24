/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_search_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 21:01:13 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static void	write_path_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

static char	*handle_err(char *result, char *cmd, char *token,
		char *message)
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

char	*handle_absolute_path_result(char *result, char *cmd)
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

char	*handle_no_path_env(char *cmd)
{
	if (access(cmd, F_OK) == 0)
		return (ft_strdup(cmd));
	return (NULL);
}
