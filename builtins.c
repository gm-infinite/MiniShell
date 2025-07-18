/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 21:30:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/18 21:41:43 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_new.h"

static int	is_valid_var_name(const char *name)
{
	int	i;

	if (!name || !name[0] || (!ft_isalpha(name[0]) && name[0] != '_'))
		return (0);
	i = 1;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	return (ft_strncmp(cmd, "echo", 5) == 0 || ft_strncmp(cmd, "cd", 3) == 0 ||
			ft_strncmp(cmd, "pwd", 4) == 0 || ft_strncmp(cmd, "export", 7) == 0 ||
			ft_strncmp(cmd, "unset", 6) == 0 || ft_strncmp(cmd, "env", 4) == 0 ||
			ft_strncmp(cmd, "exit", 5) == 0);
}

static int	builtin_echo(char **args)
{
	int	i = 1;
	int	newline = 1;

	while (args[i] && args[i][0] == '-' && args[i][1] == 'n')
	{
		int j = 1;
		while (args[i][j] == 'n')
			j++;
		if (args[i][j] == '\0')
		{
			newline = 0;
			i++;
		}
		else
			break;
	}
	while (args[i])
	{
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
	return (0);
}

static int	builtin_pwd(void)
{
	char	*cwd = getcwd(NULL, 0);
	
	if (!cwd)
		return (perror("pwd"), 1);
	printf("%s\n", cwd);
	free(cwd);
	return (0);
}

static int	builtin_cd(char **args, t_shell *shell)
{
	char	*target;
	char	*home;
	char	*oldpwd;
	char	cwd[PATH_MAX];

	if (!args[1] || ft_strncmp(args[1], "~", 2) == 0)
	{
		home = get_env_value("HOME", shell);
		target = home ? home : "/";
	}
	else
		target = args[1];
	
	if (getcwd(cwd, sizeof(cwd)))
	{
		oldpwd = ft_strdup(cwd);
		set_env_var("OLDPWD", oldpwd, shell);
		free(oldpwd);
	}
	
	if (chdir(target) == -1)
		return (perror("cd"), 1);
	
	if (getcwd(cwd, sizeof(cwd)))
		set_env_var("PWD", cwd, shell);
	return (0);
}

static int	builtin_env(t_shell *shell)
{
	int	i = 0;
	
	while (shell->envp[i])
		printf("%s\n", shell->envp[i++]);
	return (0);
}

static int	builtin_export(char **args, t_shell *shell)
{
	int		i = 1;
	char	*eq_pos;
	char	*var_name;
	char	*var_value;
	int		result = 0;

	if (!args[1])
	{
		builtin_env(shell);
		return (0);
	}
	
	while (args[i])
	{
		eq_pos = ft_strchr(args[i], '=');
		if (eq_pos)
		{
			*eq_pos = '\0';
			var_name = args[i];
			var_value = eq_pos + 1;
		}
		else
		{
			var_name = args[i];
			var_value = "";
		}
		
		if (!is_valid_var_name(var_name))
		{
			printf("export: `%s': not a valid identifier\n", var_name);
			result = 1;
		}
		else
			set_env_var(var_name, var_value, shell);
		
		if (eq_pos)
			*eq_pos = '=';
		i++;
	}
	return (result);
}

static int	builtin_unset(char **args, t_shell *shell)
{
	int	i = 1;
	int	result = 0;

	while (args[i])
	{
		if (!is_valid_var_name(args[i]))
		{
			printf("unset: `%s': not a valid identifier\n", args[i]);
			result = 1;
		}
		else
			unset_env_var(args[i], shell);
		i++;
	}
	return (result);
}

static int	builtin_exit(char **args, t_shell *shell)
{
	int	exit_code = shell->past_exit_status;

	if (args[1])
	{
		if (args[2])
		{
			printf("exit: too many arguments\n");
			return (1);
		}
		exit_code = ft_atoi(args[1]);
	}
	
	shell->should_exit = 1;
	shell->exit_code = exit_code;
	return (exit_code);
}

int	execute_builtin(char **args, t_shell *shell)
{
	if (!args[0])
		return (1);
	
	if (ft_strncmp(args[0], "echo", 5) == 0)
		return (builtin_echo(args));
	if (ft_strncmp(args[0], "cd", 3) == 0)
		return (builtin_cd(args, shell));
	if (ft_strncmp(args[0], "pwd", 4) == 0)
		return (builtin_pwd());
	if (ft_strncmp(args[0], "export", 7) == 0)
		return (builtin_export(args, shell));
	if (ft_strncmp(args[0], "unset", 6) == 0)
		return (builtin_unset(args, shell));
	if (ft_strncmp(args[0], "env", 4) == 0)
		return (builtin_env(shell));
	if (ft_strncmp(args[0], "exit", 5) == 0)
		return (builtin_exit(args, shell));
	
	return (1);
}
