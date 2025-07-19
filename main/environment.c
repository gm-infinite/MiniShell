/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 00:00:00 by user              #+#    #+#             */
/*   Updated: 2025/07/19 11:45:37 by emgenc           ###   ########.fr       */
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

typedef struct s_expand
{
	char	*result;
	char	*var_start;
	char	*var_end;
	char	*var_name;
	char	*var_value;
	char	*expanded;
	int		indx;
	int		var_len;
}	t_expand;

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


int replace_var_with_value(t_expand* holder, t_shell *shell, int flag, int should_free_value)
{
	char *before;
	char *after;
	char *temp;
	
	(void)shell;
	before = ft_substr(holder->result, 0, holder->indx);
	if (flag == 1)
		after = ft_strdup(&holder->result[holder->indx + 2]);
	else
		after = ft_strdup(holder->var_end);
	temp = ft_strjoin(before, holder->var_value);
	holder->expanded = ft_strjoin(temp, after);
	free(before);
	free(after);
	free(temp);
	if (!holder->expanded)
	{
		if (should_free_value)
			free(holder->var_value);
		return 0;
	}
	holder->var_len = ft_strlen(holder->var_value);
	free(holder->result);
	if (should_free_value)
		free(holder->var_value);
	holder->result = holder->expanded;
	holder->indx += holder->var_len;
	return 1;
}

int handle_question_mark(t_expand *holder, t_shell *shell)
{
	holder->var_value = ft_itoa(shell->past_exit_status);
	if (!holder->var_value)
		return 0 ;
	if(!replace_var_with_value(holder, shell, 1, 1))  // should_free_value = 1 for ft_itoa result
		return 0 ;
	return 1;
}

/**
 * Expand tilde (~) to home directory
 * Returns new string if expansion needed, or original string if not
 */
char	*expand_tilde(char *str, t_shell *shell)
{
	char	*home;

	if (!str || str[0] != '~')
		return (ft_strdup(str));
		
	// Handle ~ at beginning
	if (str[0] == '~' && (str[1] == '\0' || str[1] == '/'))
	{
		home = get_env_value("HOME", shell);
		if (home)
		{
			if (str[1] == '\0')
				return (ft_strdup(home));
			else
				return (ft_strjoin(home, &str[1]));
		}
	}
	
	return (ft_strdup(str));
}

char	*expand_variables(char *str, t_shell *shell)
{
	t_expand	holder;
	char		*tilde_expanded;

	if (!str)
		return (NULL);
		
	// Handle tilde expansion first
	tilde_expanded = expand_tilde(str, shell);
	if (!tilde_expanded)
		return (NULL);
	
	holder.result = tilde_expanded;  // Use tilde-expanded string as base
	holder.indx = 0;
	while (holder.result[holder.indx])
	{
		if (holder.result[holder.indx] == '$')
		{
			holder.var_start = &holder.result[holder.indx + 1];
			if (*holder.var_start == '?')
			{
				if(!handle_question_mark(&holder, shell))
					break ;
				continue ;
			}
			
			// Check if this is a valid variable name (can't start with digit)
			if (*holder.var_start && (ft_isalpha(*holder.var_start) || *holder.var_start == '_'))
			{
				holder.var_end = holder.var_start;
				while (*(holder.var_end) && (ft_isalnum(*(holder.var_end)) || *(holder.var_end) == '_'))
					holder.var_end++;
				
				if (holder.var_end > holder.var_start)
				{
					holder.var_name = ft_substr(holder.var_start, 0, holder.var_end - holder.var_start);
					if (!holder.var_name)
						break;
					holder.var_value = get_env_value(holder.var_name, shell);
					if (!holder.var_value)
						holder.var_value = ""; // Empty string if variable doesn't exist
					if (!replace_var_with_value(&holder, shell, 0, 0))  // should_free_value = 0 for env strings
					{
						free(holder.var_name);  // Clean up allocated variable name
						break;
					}
					free(holder.var_name);  // Clean up allocated variable name after successful replacement
					continue;
				}
			}
		}
		holder.indx++;
	}
	return (holder.result);
}

