/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:15:45 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 13:27:31 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	builtin_pwd(void)
{
	char	cwd[PATH_MAX];

	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("pwd");
		return (1);
	}
	printf("%s\n", cwd);
	return (0);
}

int	is_valid_var_name(const char *name)
{
	int	i;

	if (!name || !name[0])
		return (0);
	if (!ft_isalpha(name[0]) && name[0] != '_')
		return (0);
	i = 0;
	while (name[++i])
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
	return (1);
}

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strncmp(cmd, "echo", 5) == 0 && ft_strlen(cmd) == 4)
		return (1);
	if (ft_strncmp(cmd, "cd", 3) == 0 && ft_strlen(cmd) == 2)
		return (1);
	if (ft_strncmp(cmd, "pwd", 4) == 0 && ft_strlen(cmd) == 3)
		return (1);
	if (ft_strncmp(cmd, "export", 7) == 0 && ft_strlen(cmd) == 6)
		return (1);
	if (ft_strncmp(cmd, "unset", 6) == 0 && ft_strlen(cmd) == 5)
		return (1);
	if (ft_strncmp(cmd, "env", 4) == 0 && ft_strlen(cmd) == 3)
		return (1);
	if (ft_strncmp(cmd, "exit", 5) == 0 && ft_strlen(cmd) == 4)
		return (1);
	return (0);
}

int	execute_builtin(char **args, t_shell *shell)
{
	if (!args || !args[0])
		return (1);
	if (ft_strncmp(args[0], "echo", 5) == 0)
		return (builtin_echo(args));
	else if (ft_strncmp(args[0], "cd", 3) == 0)
		return (builtin_cd(args, shell));
	else if (ft_strncmp(args[0], "pwd", 4) == 0)
		return (builtin_pwd());
	else if (ft_strncmp(args[0], "export", 7) == 0)
		return (builtin_export(args, shell));
	else if (ft_strncmp(args[0], "unset", 6) == 0)
		return (builtin_unset(args, shell));
	else if (ft_strncmp(args[0], "env", 4) == 0)
		return (builtin_env(shell));
	else if (ft_strncmp(args[0], "exit", 5) == 0)
		return (builtin_exit(args, shell));
	return (1);
}
