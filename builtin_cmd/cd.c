/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:30:39 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 19:46:49 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	path_error_checking(char **args, char **path, char **home,
							t_shell *shell)
{
	if (args[1] && args[2])
	{
		write(STDERR_FILENO, "cd: too many arguments\n", 23);
		return (1);
	}
	if (!args[1] || (args[1][0] == '~' && args[1][1] == '\0'))
		return (handle_home_path(path, home, shell));
	else if (args[1][0] == '-' && args[1][1] == '\0')
		return (handle_oldpwd_path(path, shell));
	else if (ft_strncmp(args[1], "--", 2) == 0 && args[1][2] == '\0')
		return (handle_home_path(path, home, shell));
	else
		*path = args[1];
	return (0);
}

static int	change_directory(char *path)
{
	char	*errmsg;

	if (chdir(path) == -1)
	{
		errmsg = ft_strjoin("cd: ", path);
		perror(errmsg);
		free(errmsg);
		return (1);
	}
	return (0);
}

static void	update_pwd_vars(char *old_pwd, t_shell *shell)
{
	char	cwd[PATH_MAX];

	if (old_pwd)
	{
		set_env_var("OLDPWD", old_pwd, shell);
		free(old_pwd);
	}
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		set_env_var("PWD", cwd, shell);
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
	old_pwd = ft_strdup(cwd);
	if (change_directory(path))
	{
		free(old_pwd);
		return (1);
	}
	update_pwd_vars(old_pwd, shell);
	return (0);
}
