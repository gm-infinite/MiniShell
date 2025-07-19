/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:40:47 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/19 17:15:40 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

/**
 * Deep-copies a t_split structure to a char** array using ft_strdup continuously. char** arrays are suitable for execve().
 * If initial array allocation fails, returns NULL immediately. Also returns NULL-terminated array as required by POSIX exec* functions.
 * This function is called before every execve() to prepare arguments.
 * The returned array is completely independent of the input split structure.
 * Caller side is responsible for freeing the returned array with free_args()!
 */
char	**split_to_args(t_split split)
{
	char	**args;
	int		i;
	int		j;

	args = malloc(sizeof(char *) * (split.size + 1));
	if (!args)
		return (NULL);
	i = 0;
	j = 0;
	while (i < split.size)
	{
		if (split.start[i] && split.start[i][0] != '\0')
		{
			// Skip arguments that are only whitespace
			int k = 0;
			while (split.start[i][k] && (split.start[i][k] == ' ' || split.start[i][k] == '\t'))
				k++;
			if (split.start[i][k] == '\0')
			{
				i++;
				continue;
			}
			
			args[j] = ft_strdup(split.start[i]);
			if (!args[j])
			{
				free_args(args);
				return (NULL);
			}
			j++;
		}
		i++;
	}
	args[j] = NULL;
	return (args);
}

/**
 * Filters out empty strings from a char** array before execve
 * Returns a new array with empty arguments removed
 */
char	**filter_empty_args(char **args)
{
	int		count;
	int		i;
	int		j;
	char	**filtered;

	if (!args)
		return (NULL);
	
	// Count non-empty arguments
	count = 0;
	i = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')
			count++;
		i++;
	}
	
	// Allocate new array
	filtered = malloc(sizeof(char *) * (count + 1));
	if (!filtered)
		return (NULL);
	
	// Copy non-empty arguments
	i = 0;
	j = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')
		{
			filtered[j] = ft_strdup(args[i]);
			if (!filtered[j])
			{
				free_args(filtered);
				return (NULL);
			}
			j++;
		}
		i++;
	}
	filtered[j] = NULL;
	return (filtered);
}

/**
 * Safely deallocates a NULL-terminated char** array.
 * Always call this function to free arrays returned by split_to_args()!
 * This function is safe to call with NULL pointer (no-op).
 * Should be called in all cleanup paths and error handling. 
 */
void	free_args(char **args)
{
	int	i;
	if (!args)
		return ;
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

/*
** ============================================================================
** EXECUTABLE RESOLUTION AND PATH SEARCH
** ============================================================================
*/

/**
 * @brief Locates an executable file for a given command name.
 * 		If command contains '/', treat as path. Check if file exists and is accessible. Return duplicated path if valid, NULL if not found
 * 
 * 2. PATH ENVIRONMENT SEARCH: For commands without path separators
 *    - Retrieve PATH environment variable from shell environment
 *    - Split PATH by ':' delimiter to get individual directories
 *    - For each directory, construct full path and test accessibility
 *    - Return first match found, or NULL if no executable located
 * 
 * SECURITY CONSIDERATIONS:
 * - Uses access() with F_OK to verify file existence before execution
 * - Does not perform additional permission checks (handled by execve)
 * - PATH traversal follows standard shell security model
 * - No validation of executable permissions (delegated to execve)
 * 
 * MEMORY MANAGEMENT:
 * - Returns dynamically allocated string that caller must free
 * - All intermediate allocations are properly cleaned up
 * - PATH splitting array is freed before function return
 * - Temporary string constructions are freed immediately after use
 * 
 * ERROR HANDLING:
 * - NULL PATH environment variable results in NULL return
 * - Memory allocation failures return NULL
 * - File access failures continue search through remaining paths
 * - No executable found in any PATH directory returns NULL
 * 
 * POSIX COMPLIANCE:
 * - Follows standard PATH resolution behavior
 * - Treats empty PATH elements as current directory (if implemented)
 * - Uses ':' as PATH separator as per POSIX specification
 * 
 * @param cmd Command name to resolve (e.g., "ls", "/bin/cat", "./script")
 * @param shell Shell instance containing environment variables
 * @return char* Allocated full path to executable, or NULL if not found
 * 
 * @note Returned string must be freed by caller
 * @note Function may return NULL for various failure conditions
 */
char	*find_executable(char *cmd, t_shell *shell)
{
	char	*path_env;      // PATH environment variable content
	char	**paths;        // Array of PATH directories
	char	*full_path;     // Constructed full path for testing
	char	*temp;          // Temporary string for path construction
	int		i;              // Iterator for PATH directory traversal

	// PHASE 1: Handle absolute and relative paths
	// If command contains '/', it's already a complete or relative path
	// In this case, we don't search PATH - just verify the file exists
	if (ft_strchr(cmd, '/'))
	{
		// Test if the specified path exists and is accessible
		if (access(cmd, F_OK) == 0)
			return (ft_strdup(cmd));  // Return copy of the provided path
		return (NULL);                // File doesn't exist at specified path
	}
	
	// PHASE 1.5: Special handling for directory names without path separators
	// Commands like ".." should be treated as "command not found" in bash
	// Only check if it's a directory that exists in current dir but has no execute permission
	if (access(cmd, F_OK) == 0)
	{
		struct stat file_stat;
		if (stat(cmd, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
		{
			// For pure directory names like "..", ".", return NULL to trigger "command not found"
			// This matches bash behavior where ".." gives "command not found"
			if ((ft_strncmp(cmd, "..", 3) == 0 && ft_strlen(cmd) == 2) || 
				(ft_strncmp(cmd, ".", 2) == 0 && ft_strlen(cmd) == 1))
				return (NULL);
		}
	}
	
	// PHASE 2: PATH environment variable resolution
	// Retrieve PATH environment variable for directory search
	path_env = get_env_value("PATH", shell);
	if (!path_env)
	{
		// When PATH is unset, check current directory (bash behavior)
		// This allows commands like "ls" to work when cd'ed to /bin and PATH is unset
		if (access(cmd, F_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);    // No PATH variable and not in current directory
	}
	
	// Split PATH into individual directory components
	// PATH format: "/usr/bin:/bin:/usr/local/bin:..."
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);    // Memory allocation failure during PATH parsing
	
	// PHASE 3: Sequential directory search
	// Try each PATH directory until we find the executable
	i = 0;
	while (paths[i])
	{
		// Construct full path: directory + "/" + command_name
		temp = ft_strjoin(paths[i], "/");          // Add trailing slash
		full_path = ft_strjoin(temp, cmd);         // Append command name
		free(temp);                                // Clean up intermediate string
		
		// Test if constructed path points to existing file
		if (full_path && access(full_path, F_OK) == 0)
		{
			// Found executable! Clean up and return successful path
			free_args(paths);
			return (full_path);
		}
		
		// This path didn't work, clean up and try next directory
		free(full_path);
		i++;
	}
	
	// PHASE 4: Cleanup and failure return
	// No executable found in any PATH directory
	free_args(paths);
	return (NULL);
}


/**
 * @brief Detects presence of redirection operators in command arguments
 * 
 * This function performs a linear scan through the command arguments to
 * detect any redirection operators. It serves as a classifier function
 * that helps the main execution engine decide which execution path to take.
 * 
 * SUPPORTED REDIRECTION OPERATORS:
 * - ">" : Standard output redirection
 * - "<" : Standard input redirection  
 * - ">>" : Append output redirection
 * - "2>" : Standard error redirection
 * - "2>>" : Append error redirection
 * - "&>" : Combined stdout/stderr redirection
 * - And any other operators defined in is_redirection()
 * 
 * DETECTION ALGORITHM:
 * - Iterates through all arguments in the split structure
 * - Calls is_redirection() for each argument to test for operators
 * - Returns immediately upon finding first redirection operator
 * - Returns 0 only if no redirections found in entire command
 * 
 * PERFORMANCE CONSIDERATIONS:
 * - Early termination on first redirection found (short-circuit evaluation)
 * - Linear time complexity O(n) where n is number of arguments
 * - Minimal overhead for commands without redirections
 * 
 * @param split Command arguments structure to scan
 * @return int 1 if redirections found, 0 if no redirections present
 * 
 * @note This function only detects presence, not validity of redirections
 * @note Actual redirection parsing and validation occurs in separate functions
 */

int	has_redirections(t_split split)
{
	int	i;      // Iterator for argument scanning

	// Scan through all arguments looking for redirection operators
	i = 0;
	while (i < split.size)
	{
		// Test if current argument is a redirection operator
		if (is_redirection(split.start[i]))
			return (1);     // Found redirection - early return
		i++;
	}
	
	// No redirections found in any argument
	return (0);
}

/*
** ============================================================================
** ARGUMENT ARRAY COMPACTION UTILITIES
** ============================================================================
*/


/**
 * This function removes empty strings from the arguments array and shifts
 * remaining arguments to fill the gaps. This is necessary after variable
 * expansion where undefined variables become empty strings.
 * 
 * For example: ["$EMPTY", "echo", "hi"] becomes ["echo", "hi"] 
 * when $EMPTY expands to an empty string.
 */
void	compact_args(char **args)
{
	int	read_pos;   // Position we're reading from
	int	write_pos;  // Position we're writing to
	
	if (!args)
		return;
	read_pos = 0;
	write_pos = 0;
	// Compact the array by removing empty strings
	while (args[read_pos])
	{
		if (args[read_pos][0] != '\0')  // Non-empty string
		{
			if (read_pos != write_pos)
			{
				args[write_pos] = args[read_pos];
				args[read_pos] = NULL;  // Clear the old position
			}
			write_pos++;
		}
		else  // Empty string - free it and skip
		{
			free(args[read_pos]);
			args[read_pos] = NULL;
		}
		read_pos++;
	}
	args[write_pos] = NULL;
}
