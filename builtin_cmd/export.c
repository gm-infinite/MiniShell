/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:36:19 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/16 20:58:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	set_env_var(char *var_name, char *value, t_shell *shell)
{
	int		i;
	int		len;
	char	*eq_pos;
	char	**new_envp;

	if (!var_name || !value)
		return (1);
	len = ft_strlen(var_name);
	i = 0;
	while (shell->envp[i])
	{
		eq_pos = ft_strchr(shell->envp[i], '=');
		if (eq_pos && (eq_pos - shell->envp[i]) == len)
		{
			if (ft_strncmp(shell->envp[i], var_name, len) == 0)
			{
				char *temp = ft_strjoin(var_name, "=");
				if (!temp)
					return (1);
				free(shell->envp[i]);
				shell->envp[i] = ft_strjoin(temp, value);
				free(temp);
				if (!shell->envp[i])
					return (1);
				return (0);
			}
		}
		i++;
	}
	new_envp = malloc(sizeof(char *) * (i + 2));
	if (!new_envp)
		return (1);
	ft_memcpy(new_envp, shell->envp, sizeof(char *) * i);
	char *temp = ft_strjoin(var_name, "=");
	if (!temp)
	{
		free(new_envp);
		return (1);
	}
	new_envp[i] = ft_strjoin(temp, value);
	free(temp);
	if (!new_envp[i])
	{
		free(new_envp);
		return (1);
	}
	new_envp[i + 1] = NULL;
	free(shell->envp);
	shell->envp = new_envp;
	return (0);
}

int	builtin_export(char **args, t_shell *shell)
{
	int		i;
	char	*eq_pos;
	char	*var_name;
	char	*var_value;
	int		exit_status = 0;  // Track exit status for this command

	// No arguments: print all environment variables in sorted order
	if (!args[1])
	{
		// ALPHABETICAL SORTING FOR EXPORT OUTPUT
		// Count environment variables for array allocation
		int count = 0;
		while (shell->envp[count])
			count++;
		
		// Create array of pointers for sorting
		char **sorted_env = malloc(sizeof(char *) * count);
		if (!sorted_env)
			return (1);
		
		// Copy environment pointers
		for (i = 0; i < count; i++)
			sorted_env[i] = shell->envp[i];
		
		// Simple bubble sort for alphabetical order
		// (Efficient enough for typical environment size)
		for (i = 0; i < count - 1; i++)
		{
			for (int j = 0; j < count - i - 1; j++)
			{
				if (ft_strncmp(sorted_env[j], sorted_env[j + 1], 
					ft_strlen(sorted_env[j])) > 0)
				{
					// Swap elements
					char *temp = sorted_env[j];
					sorted_env[j] = sorted_env[j + 1];
					sorted_env[j + 1] = temp;
				}
			}
		}
		
		// Print sorted environment with declare -x format
		for (i = 0; i < count; i++)
		{
			printf("declare -x %s\n", sorted_env[i]);
		}
		
		// Cleanup
		free(sorted_env);
		return (0);
	}
	
	// Process each argument
	i = 1;
	while (args[i])
	{
		eq_pos = ft_strchr(args[i], '=');
		if (eq_pos)
		{
			// name=value format - extract and validate name
			var_name = ft_substr(args[i], 0, eq_pos - args[i]);
			if (!var_name)
			{
				i++;
				continue;
			}
			
			// Validate variable name
			if (!is_valid_var_name(var_name))
			{
				// Print error message to stderr like bash
				write(STDERR_FILENO, "export: `", 9);
				write(STDERR_FILENO, args[i], ft_strlen(args[i]));
				write(STDERR_FILENO, "': not a valid identifier\n", 26);
				free(var_name);
				i++;
				exit_status = 1;  // Set error exit status for this command
				continue;
			}
			
			var_value = ft_strdup(eq_pos + 1);
			if (var_value)
				set_env_var(var_name, var_value, shell);
			
			free(var_name);
			free(var_value);
		}
		else
		{
			// Just name - validate first
			if (!is_valid_var_name(args[i]))
			{
				// Print error message to stderr like bash
				write(STDERR_FILENO, "export: `", 9);
				write(STDERR_FILENO, args[i], ft_strlen(args[i]));
				write(STDERR_FILENO, "': not a valid identifier\n", 26);
				i++;
				exit_status = 1;  // Set error exit status for this command
				continue;
			}
			
			// Set to empty value
			set_env_var(args[i], "", shell);
		}
		i++;
	}
	
	// Update shell's exit status and return
	shell->past_exit_status = exit_status;
	return (exit_status);
}
