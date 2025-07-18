/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 21:35:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/18 21:41:43 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_new.h"

int	is_empty(char *str)
{
	int	i = 0;
	
	if (!str)
		return (1);
	while (str[i])
	{
		if (!(str[i] == ' ' || (str[i] <= 13 && str[i] >= 9)))
			return (0);
		i++;
	}
	return (1);
}

void	free_args(char **args)
{
	int	i = 0;
	
	if (!args)
		return;
	while (args[i])
		free(args[i++]);
	free(args);
}

char	**split_to_args(t_split split)
{
	char	**args = malloc(sizeof(char *) * (split.size + 1));
	int		i = 0;
	
	if (!args)
		return (NULL);
	while (i < split.size)
	{
		args[i] = ft_strdup(split.start[i]);
		i++;
	}
	args[i] = NULL;
	return (args);
}

void	process_args_quotes(char **args, t_shell *shell)
{
	int	i = 0;
	
	while (args[i])
	{
		char *processed = process_quotes(args[i], shell);
		if (processed != args[i])
		{
			free(args[i]);
			args[i] = processed;
		}
		i++;
	}
}

void	compact_args(char **args)
{
	int	i = 0, j = 0;
	
	while (args[i])
	{
		if (args[i][0] != '\0')
		{
			if (i != j)
			{
				args[j] = args[i];
				args[i] = NULL;
			}
			j++;
		}
		else
		{
			free(args[i]);
			args[i] = NULL;
		}
		i++;
	}
	args[j] = NULL;
}

char	*find_executable(char *cmd, t_shell *shell)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	int		i = 0;
	
	if (!cmd || cmd[0] == '\0')
		return (NULL);
	
	if (cmd[0] == '/' || cmd[0] == '.')
		return (access(cmd, X_OK) == 0 ? ft_strdup(cmd) : NULL);
	
	path_env = get_env_value("PATH", shell);
	if (!path_env)
		return (NULL);
	
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	
	while (paths[i])
	{
		full_path = malloc(ft_strlen(paths[i]) + ft_strlen(cmd) + 2);
		if (full_path)
		{
			ft_strlcpy(full_path, paths[i], ft_strlen(paths[i]) + 1);
			ft_strlcat(full_path, "/", ft_strlen(paths[i]) + 2);
			ft_strlcat(full_path, cmd, ft_strlen(paths[i]) + ft_strlen(cmd) + 2);
			
			if (access(full_path, X_OK) == 0)
			{
				free_args(paths);
				return (full_path);
			}
			free(full_path);
		}
		i++;
	}
	
	free_args(paths);
	return (NULL);
}

void	shell_init(t_shell *shell)
{
	shell->current_input = NULL;
	shell->split_input = create_split(NULL, 0);
	shell->past_exit_status = 0;
	shell->should_exit = 0;
	shell->exit_code = 0;
	shell->envp = NULL;
}

void	safe_exit(t_shell *shell)
{
	if (shell->current_input)
		free(shell->current_input);
	if (shell->split_input.start)
		free_split(&(shell->split_input));
	free_environment(shell);
	rl_clear_history();
	exit(shell->past_exit_status);
}

int	count_pipes(t_split split)
{
	int	count = 0;
	int	i = 0;
	
	while (i < split.size)
	{
		if (ft_strncmp(split.start[i], "|", 2) == 0)
			count++;
		i++;
	}
	return (count);
}

t_split	*split_by_pipes(t_split split, int *cmd_count)
{
	int		pipe_count = count_pipes(split);
	t_split	*commands = malloc(sizeof(t_split) * (pipe_count + 1));
	int		cmd_idx = 0;
	int		start = 0;
	int		i = 0;
	
	*cmd_count = pipe_count + 1;
	if (!commands)
		return (NULL);
	
	while (i <= split.size)
	{
		if (i == split.size || ft_strncmp(split.start[i], "|", 2) == 0)
		{
			commands[cmd_idx] = create_split(&split.start[start], i - start);
			cmd_idx++;
			start = i + 1;
		}
		i++;
	}
	return (commands);
}

int	is_redirection(char *token)
{
	return (ft_strncmp(token, "<", 2) == 0 || ft_strncmp(token, ">", 2) == 0 ||
			ft_strncmp(token, "<<", 3) == 0 || ft_strncmp(token, ">>", 3) == 0);
}

int	has_redirections(t_split split)
{
	int	i = 0;
	
	while (i < split.size)
	{
		if (is_redirection(split.start[i]))
			return (1);
		i++;
	}
	return (0);
}
