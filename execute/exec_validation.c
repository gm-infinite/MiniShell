/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_validation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:10:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 22:15:09 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	validate_and_process_args(char **args, t_shell *shell)
{
	int	i;

	if (!args || !args[0])
		return (1);
	i = 0;
	while (args[i])
	{
		args[i] = expand_variables_quoted(args[i], shell);
		i++;
	}
	compact_args(args);
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

int	validate_executable(char *cmd, char *executable)
{
    struct stat	file_stat;

    if (stat(executable, &file_stat) == 0)
    {
        if (S_ISDIR(file_stat.st_mode))
        {
            fprintf(stderr, "%s: Is a directory\n", cmd);
            return (126);
        }
        if (access(executable, X_OK) != 0)
        {
            fprintf(stderr, "%s: Permission denied\n", cmd);
            return (126);
        }
    }
    return (0);
}

int	handle_executable_not_found(char **args)
{
    if (ft_strchr(args[0], '/'))
        fprintf(stderr, "%s: No such file or directory\n", args[0]);
    else
        fprintf(stderr, "%s: command not found\n", args[0]);
    return (127);
}
