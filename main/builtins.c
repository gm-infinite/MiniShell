/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 00:00:00 by user              #+#    #+#             */
/*   Updated: 2025/07/13 08:00:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** ============================================================================
**                              BUILTIN COMMANDS
** ============================================================================
**
** This file implements the shell's built-in commands system, providing
** essential shell functionality that must be executed within the shell
** process rather than as external programs.
**
** CORE FUNCTIONALITY:
** - Built-in command identification and validation
** - Direct execution of built-in commands
** - Integration with shell state and environment
** - Proper exit status handling for built-ins
**
** SUPPORTED BUILT-INS:
** 1. echo    - Output text with optional newline control
** 2. cd      - Change current directory
** 3. pwd     - Print working directory
** 4. export  - Set environment variables
** 5. unset   - Remove environment variables
** 6. env     - Display environment variables
** 7. exit    - Terminate shell with optional exit code
**
** DESIGN PRINCIPLES:
** - Performance: Fast built-in identification using exact string matching
** - Reliability: Robust error handling for all built-in operations
** - Compatibility: Behavior consistent with bash built-ins
** - State Management: Proper integration with shell environment and state
** - Memory Safety: Careful handling of dynamic memory in environment operations
**
** INTEGRATION CONTEXT:
** Built-ins are executed directly by the executor when identified, bypassing
** the normal fork/exec process creation. This is essential for commands that
** must modify the shell's state (like cd, export) or access shell internals.
*/

#include "minishell.h"

/*
** ============================================================================
**                         BUILT-IN IDENTIFICATION
** ============================================================================
*/

/**
** is_builtin - Determines if a command string represents a shell built-in
**
** This function performs fast identification of built-in commands using
** exact string matching with length validation. The dual check (content and length)
** prevents false positives from commands that start with built-in names.
**
** ALGORITHM:
** 1. Null pointer validation for safety
** 2. Exact string comparison using ft_strncmp
** 3. Length validation using ft_strlen for precision
** 4. Return 1 for built-ins, 0 for external commands
**
** PERFORMANCE CONSIDERATIONS:
** - Uses strncmp with exact length checks for O(1) average case
** - No dynamic memory allocation
** - Early return on first match
** - Optimized for most common built-ins first
**
** EDGE CASES HANDLED:
** - NULL command pointer
** - Empty command string
** - Commands that are prefixes of built-in names
** - Commands that contain built-in names as substrings
**
** @param cmd: Command string to test (may be NULL)
** @return: 1 if command is a built-in, 0 otherwise
**
** EXAMPLES:
** - is_builtin("echo") returns 1
** - is_builtin("echo123") returns 0
** - is_builtin("echoing") returns 0
** - is_builtin(NULL) returns 0
*/
int	is_builtin(char *cmd)
{
	// INPUT VALIDATION
	// Protect against null pointer dereferencing
	if (!cmd)
		return (0);
	
	// EXACT MATCHING WITH LENGTH VALIDATION
	// Each check verifies both content and exact length to prevent false positives
	
	// Check for 'echo' command (4 characters exactly)
	if (ft_strncmp(cmd, "echo", 5) == 0 && ft_strlen(cmd) == 4)
		return (1);
		
	// Check for 'cd' command (2 characters exactly)
	if (ft_strncmp(cmd, "cd", 3) == 0 && ft_strlen(cmd) == 2)
		return (1);
		
	// Check for 'pwd' command (3 characters exactly)
	if (ft_strncmp(cmd, "pwd", 4) == 0 && ft_strlen(cmd) == 3)
		return (1);
		
	// Check for 'export' command (6 characters exactly)
	if (ft_strncmp(cmd, "export", 7) == 0 && ft_strlen(cmd) == 6)
		return (1);
		
	// Check for 'unset' command (5 characters exactly)
	if (ft_strncmp(cmd, "unset", 6) == 0 && ft_strlen(cmd) == 5)
		return (1);
		
	// Check for 'env' command (3 characters exactly)
	if (ft_strncmp(cmd, "env", 4) == 0 && ft_strlen(cmd) == 3)
		return (1);
		
	// Check for 'exit' command (4 characters exactly)
	if (ft_strncmp(cmd, "exit", 5) == 0 && ft_strlen(cmd) == 4)
		return (1);
	
	// NOT A BUILT-IN
	// Command does not match any known built-in pattern
	return (0);
}

/*
** ============================================================================
**                         BUILT-IN EXECUTION
** ============================================================================
*/

/**
** execute_builtin - Executes a shell built-in command with proper state management
**
** This function serves as the central dispatcher for all built-in command execution.
** It identifies the specific built-in and delegates to specialized handler functions,
** ensuring proper shell state updates and consistent exit status handling.
**
** EXECUTION FLOW:
** 1. Input validation and safety checks
** 2. Built-in command identification
** 3. Dispatch to specific built-in handler
** 4. Shell state update if necessary
** 5. Return appropriate exit status
**
** STATE MANAGEMENT:
** - Environment variables (export/unset)
** - Current working directory (cd)
** - Shell termination flags (exit)
** - Exit status propagation
**
** ERROR HANDLING:
** - Invalid arguments to built-ins
** - System call failures (cd, pwd)
** - Memory allocation failures
** - Environment variable operations
**
** INTEGRATION NOTES:
** - Called directly by executor for identified built-ins
** - Must not fork - operates within shell process
** - Updates shell state directly
** - Returns exit status for command chaining
**
** @param args: Null-terminated argument array (args[0] is command name)
** @param shell: Shell state structure for environment and status tracking
** @return: Exit status of executed built-in (0 for success, non-zero for error)
**
** COMMAND DELEGATION:
** - echo: builtin_echo() - Text output with formatting options
** - cd: builtin_cd() - Directory navigation with error handling
** - pwd: builtin_pwd() - Current directory display
** - export: builtin_export() - Environment variable assignment
** - unset: builtin_unset() - Environment variable removal
** - env: builtin_env() - Environment variable display
** - exit: builtin_exit() - Shell termination with cleanup
*/
int	execute_builtin(char **args, t_shell *shell)
{
	// INPUT VALIDATION
	// Ensure we have valid arguments and at least a command name
	if (!args || !args[0])
		return (1);  // Return error status for invalid input
	
	// COMMAND DISPATCH WITH EXACT MATCHING
	// Route each built-in to its specialized handler function
	
	// ECHO: Text output with optional formatting control
	if (ft_strncmp(args[0], "echo", 5) == 0)
		return (builtin_echo(args));
		
	// CD: Directory navigation with path resolution
	else if (ft_strncmp(args[0], "cd", 3) == 0)
		return (builtin_cd(args, shell));
		
	// PWD: Current working directory display
	else if (ft_strncmp(args[0], "pwd", 4) == 0)
		return (builtin_pwd());
		
	// EXPORT: Environment variable assignment and display
	else if (ft_strncmp(args[0], "export", 7) == 0)
		return (builtin_export(args, shell));
		
	// UNSET: Environment variable removal
	else if (ft_strncmp(args[0], "unset", 6) == 0)
		return (builtin_unset(args, shell));
	else if (ft_strncmp(args[0], "env", 4) == 0)
		return (builtin_env(shell));
	else if (ft_strncmp(args[0], "exit", 5) == 0)
		return (builtin_exit(args, shell));
	
	return (1);
}

/*
** Built-in: echo [-n] [string ...]
** Prints arguments separated by single spaces, followed by newline (unless -n)
*/
int	builtin_echo(char **args)
{
	int	i;
	int	newline;

	newline = 1;
	i = 1;
	
	// Check for -n flag(s)
	while (args[i] && args[i][0] == '-')
	{
		if (args[i][1] == 'n')
		{
			int j = 1;
			while (args[i][j] == 'n')
				j++;
			if (args[i][j] == '\0')
			{
				newline = 0;
				i++;
				continue;
			}
		}
		break;
	}
	
	// Print arguments
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

/*
** Built-in: cd [directory]
** Changes current working directory
*/
int	builtin_cd(char **args, t_shell *shell)
{
	char	*path;
	char	*home;
	char	cwd[PATH_MAX];

	// Check for too many arguments (cd accepts 0 or 1 argument only)
	if (args[1] && args[2])
	{
		write(STDERR_FILENO, "cd: too many arguments\n", 23);
		return (1);
	}

	// No argument or ~ means go to HOME
	if (!args[1] || (args[1][0] == '~' && args[1][1] == '\0'))
	{
		home = get_env_value("HOME", shell);
		if (!home)
		{
			write(STDERR_FILENO, "cd: HOME not set\n", 17);
			return (1);
		}
		path = home;
	}
	else
		path = args[1];
	
	// Get current directory for error messages
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("cd: getcwd");
		return (1);
	}
	
	// Change directory
	if (chdir(path) == -1)
	{
		perror("cd");
		return (1);
	}
	
	// Update PWD environment variable
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		set_env_var("PWD", cwd, shell);
	
	return (0);
}

/*
** Built-in: pwd
** Print current working directory
*/
int	builtin_pwd(void)
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

/**
 * @brief Validates if a string is a valid shell variable name
 * 
 * According to POSIX standards, shell variable names must:
 * 1. Start with a letter (a-z, A-Z) or underscore (_)
 * 2. Contain only letters, digits (0-9), and underscores
 * 3. NOT contain hyphens, spaces, or other special characters
 * 
 * VALIDATION ALGORITHM:
 * ====================
 * - First character: Must be alphabetic or underscore
 * - Subsequent characters: Must be alphanumeric or underscore
 * - Empty strings and NULL pointers are invalid
 * 
 * EXAMPLES:
 * ========
 * Valid names: "VAR", "var", "_VAR", "VAR123", "_123"
 * Invalid names: "123VAR", "VAR-NAME", "VAR NAME", "VAR=", ""
 * 
 * @param name Variable name to validate
 * @return int 1 if valid, 0 if invalid
 * 
 * @note Function is safe with NULL input (returns 0)
 * @note Used by export command to prevent invalid variable creation
 */
static int	is_valid_var_name(const char *name)
{
	int	i;
	
	// NULL or empty string check
	if (!name || !name[0])
		return (0);
	
	// First character must be letter or underscore
	if (!ft_isalpha(name[0]) && name[0] != '_')
		return (0);
	
	// Subsequent characters must be alphanumeric or underscore
	i = 1;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

/*
** Built-in: export [name[=value] ...]
** Set environment variables
*/
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

/*
** Built-in: unset [name ...]
** Remove environment variables
*/
int	builtin_unset(char **args, t_shell *shell)
{
	int	i;

	if (!args[1])
		return (0);
	
	i = 1;
	while (args[i])
	{
		unset_env_var(args[i], shell);
		i++;
	}
	
	return (0);
}

/*
** Built-in: env
** Print environment variables
*/
int	builtin_env(t_shell *shell)
{
	int	i;

	i = 0;
	while (shell->envp[i])
	{
		printf("%s\n", shell->envp[i]);
		i++;
	}
	
	return (0);
}

/*
** Built-in: exit [n]
** Exit the shell with optional exit code
*/
int	builtin_exit(char **args, t_shell *shell)
{
	int	exit_code;

	printf("exit\n");
	
	// Check for too many arguments (exit accepts 0 or 1 argument only)
	if (args[1] && args[2])
	{
		write(STDERR_FILENO, "exit: too many arguments\n", 25);
		return (1);
	}
	
	if (args[1])
	{
		exit_code = ft_atoi(args[1]);
		// Check if it's a valid number
		if (!ft_isdigit(args[1][0]) && args[1][0] != '-' && args[1][0] != '+')
		{
			write(STDERR_FILENO, "exit: numeric argument required\n", 32);
			exit_code = 2;
		}
	}
	else
		exit_code = shell->past_exit_status;
	
	// Set exit flag instead of calling exit() directly
	shell->should_exit = 1;
	shell->exit_code = exit_code;
	return (exit_code);
}
