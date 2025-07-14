/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 00:00:00 by user              #+#    #+#             */
/*   Updated: 2025/07/13 19:52:25 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** ============================================================================
**                          ENVIRONMENT MANAGEMENT
** ============================================================================
**
** This file implements comprehensive environment variable management for the
** minishell, providing dynamic manipulation of the shell's environment space.
** It supports all standard shell operations on environment variables with
** proper memory management and error handling.
**
** CORE FUNCTIONALITY:
** - Environment initialization from system environment
** - Dynamic environment variable creation and modification
** - Environment variable retrieval and search operations
** - Environment variable deletion and cleanup
** - Memory management for dynamic environment arrays
**
** KEY FEATURES:
** 1. DYNAMIC MEMORY MANAGEMENT
**    - Automatic array resizing for new variables
**    - Proper cleanup to prevent memory leaks
**    - Safe string duplication and manipulation
**
** 2. VARIABLE OPERATIONS
**    - Set/update variables (export command support)
**    - Retrieve variable values (expansion support)
**    - Remove variables (unset command support)
**    - List all variables (env command support)
**
** 3. FORMAT HANDLING
**    - Standard VAR=value format parsing
**    - Proper handling of empty values
**    - Edge case management for special characters
**
** MEMORY ARCHITECTURE:
** The environment is stored as a null-terminated array of strings (char **envp),
** where each string follows the format "NAME=value". The array is dynamically
** resized when variables are added or removed, maintaining contiguous memory
** layout for efficient traversal and compatibility with execve().
**
** INTEGRATION CONTEXT:
** - Used by export/unset built-ins for variable manipulation
** - Provides variable expansion support for command parsing
** - Maintains environment state across command execution
** - Integrates with process creation for proper environment inheritance
*/

#include "minishell.h"

/*
** ============================================================================
**                        ENVIRONMENT INITIALIZATION
** ============================================================================
** Some post-conditions after execution:
** - shell->envp contains complete copy of system environment
** - Original envp remains unchanged!
**
** EXAMPLE:
** System env: ["PATH=/bin", "HOME=/home/user", NULL]
** Result: shell->envp = ["PATH=/bin", "HOME=/home/user", NULL] (copied)
*/
void	init_environment(t_shell *shell, char **envp)
{
	int	i;
	int	count;

	count = 0;
	while (envp[count])
		count++;
	shell->envp = malloc(sizeof(char *) * (count + 1));
	if (!shell->envp)
		return ;
	i = 0;
	while (i < count)
	{
		shell->envp[i] = ft_strdup(envp[i]);
		i++;
	}
	shell->envp[i] = NULL;
}


char	*get_env_value(char *var_name, t_shell *shell)
{
	int		i;
	int		len;
	char	*eq_pos;

	if (!var_name || !shell->envp)
		return (NULL);
	len = ft_strlen(var_name);
	i = 0;
	while (shell->envp[i])
	{
		eq_pos = ft_strchr(shell->envp[i], '=');
		if (eq_pos && (eq_pos - shell->envp[i]) == len)
			if (ft_strncmp(shell->envp[i], var_name, len) == 0)
				return (eq_pos + 1);
		i++;
	}
	return (NULL);
}

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


int	unset_env_var(char *var_name, t_shell *shell)
{
	int		i;
	int		j;
	int		len;
	int		count;
	char	*eq_pos;
	char	**new_envp;

	if (!var_name || !shell->envp)
		return (1);
	
	len = ft_strlen(var_name);
	count = 0;
	while (shell->envp[count])
		count++;
	
	// Find and remove the variable
	i = 0;
	while (shell->envp[i])
	{
		eq_pos = ft_strchr(shell->envp[i], '=');
		if (eq_pos && (eq_pos - shell->envp[i]) == len)
		{
			if (ft_strncmp(shell->envp[i], var_name, len) == 0)
			{
				// Found variable to remove
				new_envp = malloc(sizeof(char *) * count);
				if (!new_envp)
					return (1);
				
				// Copy all except the one to remove
				j = 0;
				while (j < i)
				{
					new_envp[j] = shell->envp[j];
					j++;
				}
				free(shell->envp[i]); // Free the removed variable
				while (j < count - 1)
				{
					new_envp[j] = shell->envp[j + 1];
					j++;
				}
				new_envp[j] = NULL;
				
				free(shell->envp);
				shell->envp = new_envp;
				return (0);
			}
		}
		i++;
	}
	return (1); // Variable not found
}

/*
** Expand environment variables in a string
** Handles $VAR and $? expansion
*/
char	*expand_variables(char *str, t_shell *shell)
{
	char	*result;
	char	*var_start;
	char	*var_end;
	char	*var_name;
	char	*var_value;
	char	*expanded;
	int		i;
	int		var_len;

	if (!str)
		return (NULL);
	
	result = ft_strdup(str);
	if (!result)
		return (NULL);
	
	i = 0;
	while (result[i])
	{
		if (result[i] == '$')
		{
			var_start = &result[i + 1];
			
			// Handle $? special case
			if (*var_start == '?')
			{
				var_value = ft_itoa(shell->past_exit_status);
				if (!var_value)
					break;
				
				// Replace $? with exit status
				char *before = ft_substr(result, 0, i);
				char *after = ft_strdup(&result[i + 2]);
				char *temp = ft_strjoin(before, var_value);
				expanded = ft_strjoin(temp, after);
				free(before);
				free(after);
				free(temp);
				if (!expanded)
				{
					free(var_value);
					break;
				}
				
				var_len = ft_strlen(var_value);
				free(result);
				free(var_value);
				result = expanded;
				i += var_len;
				continue;
			}
			
			// Find end of variable name
			var_end = var_start;
			while (*var_end && (ft_isalnum(*var_end) || *var_end == '_'))
				var_end++;
			
			if (var_end > var_start)
			{
				// Extract variable name
				var_name = ft_substr(var_start, 0, var_end - var_start);
				if (!var_name)
					break;
				
				// Get variable value
				var_value = get_env_value(var_name, shell);
				if (!var_value)
					var_value = ""; // Empty string if variable doesn't exist
				
				// Replace $VAR with value
				char *before = ft_substr(result, 0, i);
				char *after = ft_strdup(var_end);
				char *temp = ft_strjoin(before, var_value);
				expanded = ft_strjoin(temp, after);
				free(before);
				free(after);
				free(temp);
				if (!expanded)
				{
					free(var_name);
					break;
				}
				
				var_len = ft_strlen(var_value);
				free(result);
				free(var_name);
				result = expanded;
				i += var_len;
				continue;
			}
		}
		i++;
	}
	
	return (result);
}

/*
** Free all environment variables
*/
void	free_environment(t_shell *shell)
{
	int	i;

	if (!shell->envp)
		return ;
	
	i = 0;
	while (shell->envp[i])
	{
		free(shell->envp[i]);
		i++;
	}
	free(shell->envp);
	shell->envp = NULL;
}
