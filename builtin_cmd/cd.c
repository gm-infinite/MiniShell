/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:30:39 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/16 20:58:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int path_error_checking(char **args, char **path, char **home, t_shell *shell)
{
	if (args[1] && args[2])
	{
		write(STDERR_FILENO, "cd: too many arguments\n", 23);
		return (1);
	}
	if (!args[1] || (args[1][0] == '~' && args[1][1] == '\0'))
	{
		*home = get_env_value("HOME", shell);
		if (!(*home))
		{
			write(STDERR_FILENO, "cd: HOME not set\n", 17);
			return (1);
		}
		*path = *home;
	}
	else
		*path = args[1];
	return (0);
}

int	builtin_cd(char **args, t_shell *shell)
{
	char	*path;
	char	*home;
	char	cwd[PATH_MAX];

	if (path_error_checking(args, &path, &home, shell))
		return (1);
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("cd: getcwd");
		return (1);
	}
	if (chdir(path) == -1)
	{
		perror("cd");
		return (1);
	}
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		set_env_var("PWD", cwd, shell);
	return (0);
}
