/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:30:39 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/19 11:45:37 by emgenc           ###   ########.fr       */
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
	else if (args[1][0] == '-' && args[1][1] == '\0')
	{
		*path = get_env_value("OLDPWD", shell);
		if (!(*path))
		{
			write(STDERR_FILENO, "cd: OLDPWD not set\n", 19);
			return (1);
		}
		// Print the directory we're going to (bash behavior)
		printf("%s\n", *path);
	}
	else if (ft_strncmp(args[1], "--", 2) == 0 && args[1][2] == '\0')
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
	char	*old_pwd;

	if (path_error_checking(args, &path, &home, shell))
		return (1);
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("cd: getcwd");
		return (1);
	}
	
	// Store current directory as OLDPWD before changing
	old_pwd = ft_strdup(cwd);
	
	if (chdir(path) == -1)
	{
		perror("cd");
		free(old_pwd);
		return (1);
	}
	
	// Set OLDPWD to previous directory
	if (old_pwd)
	{
		set_env_var("OLDPWD", old_pwd, shell);
		free(old_pwd);
	}
	
	// Update PWD to new directory
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		set_env_var("PWD", cwd, shell);
	return (0);
}
