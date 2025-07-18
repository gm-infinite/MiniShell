/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 14:15:09 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/16 20:58:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

/**
 * 
 * FUNCTIONAL OVERVIEW:
 * ===================
 * The function performs three primary operations:
 * 1. REDIRECTION EXTRACTION: Identifies and processes all redirection operators
 * 2. FILE DESCRIPTOR SETUP: Opens files and configures I/O redirections
 * 3. ARGUMENT CLEANING: Removes redirection syntax from command arguments
 * 
 * TWO-PASS ALGORITHM:
 * ==================
 * 
 * Pass 1 - Analysis Phase:
 * - Scans all arguments to identify redirection patterns
 * - Counts clean arguments (non-redirection tokens)
 * - Determines memory allocation requirements for clean argument array
 * - Handles both simple (>) and numbered (2 >) redirection patterns
 * 
 * Pass 2 - Processing Phase:
 * - Processes each redirection operator and sets up file descriptors
 * - Opens target files with appropriate modes and permissions
 * - Builds clean argument array excluding redirection syntax
 * - Validates syntax and reports errors for malformed redirections
 * 
 * REDIRECTION TYPE PROCESSING:
 * ===========================
 * 
 * Type 1 - Here Document (<<):
 * - Calls handle_here_doc() to process inline input
 * - Sets up pipe for data transfer to command stdin
 * - Handles delimiter-based input termination
 * 
 * Type 2 - Append Output (>>):
 * - Opens target file in append mode (O_APPEND)
 * - Preserves existing file content
 * - Creates file if it doesn't exist (O_CREAT)
 * 
 * Type 3 - Input Redirection (<):
 * - Opens target file for reading (O_RDONLY)
 * - Redirects command stdin to file content
 * - Reports error if file doesn't exist or isn't readable
 * 
 * Type 4 - Output Redirection (>):
 * - Opens target file for writing (O_WRONLY)
 * - Truncates existing file content (O_TRUNC)
 * - Creates file if it doesn't exist (O_CREAT)
 * 
 * Type 5 - Error Redirection (2>):
 * - Opens target file for stderr output
 * - Uses same flags as standard output redirection
 * - Redirects stderr (file descriptor 2) to file
 * 
 * ARGUMENT CLEANING STRATEGY:
 * ==========================
 * The function removes redirection syntax from command arguments:
 * - Redirection operators (<, >, >>, 2>, <<) are removed
 * - Target filenames are removed
 * - Numbered redirection patterns ("2 >") are handled specially
 * - Clean arguments are duplicated to new array for command execution
 * 
 * FILE PERMISSION MANAGEMENT:
 * ===========================
 * Created files use standard permissions (0644):
 * - Owner: read/write (6)
 * - Group: read (4)  
 * - Other: read (4)
 * - Follows standard Unix file creation practices
 * 
 * ERROR HANDLING AND VALIDATION:
 * ==============================
 * - Missing filenames trigger syntax error reporting
 * - File operation failures are reported via perror()
 * - Memory allocation failures are handled gracefully
 * - All error paths include proper cleanup of allocated resources
 * 
 * @param split Command tokens containing arguments and redirection operators
 * @param input_fd Pointer to input file descriptor (modified by function)
 * @param output_fd Pointer to output file descriptor (modified by function)  
 * @param stderr_fd Pointer to stderr file descriptor (modified by function)
 * @return char** Clean argument array with redirections removed, or NULL on failure
 * 
 * @note Function modifies file descriptor pointers to set up redirections
 * @note Returned array must be freed by caller using free_args()
 * @note File descriptors opened by function must be closed by caller
 */

void redirection_fail_procedure(int *input_fd, int *output_fd, int *stderr_fd)
{
	if (*input_fd != STDIN_FILENO && *input_fd != -1)
	{
		close(*input_fd);
		*input_fd = STDIN_FILENO;
	}
	if (*output_fd != STDOUT_FILENO && *output_fd != -1)
	{
		close(*output_fd);
		*output_fd = STDOUT_FILENO;
	}
	if (*stderr_fd != STDERR_FILENO && *stderr_fd != -1)
	{
		close(*stderr_fd);
		*stderr_fd = STDERR_FILENO;
	}
}

char	**parse_redirections(t_split split, int *input_fd, int *output_fd, int *stderr_fd)
{
	char	**args;              // Initial argument array from tokenization
	char	**clean_args;        // Clean argument array without redirection syntax
	int		i;                   // Primary iterator for argument traversal
	int		j;                   // Secondary iterator for clean argument counting/building
	int		redirect_type;       // Classification code for redirection operators
	int		here_doc_pipe[2];    // Pipe pair for here document processing
	int		redirection_failed = 0;  // Track if any redirection operation failed

	// PHASE 1: INITIAL TOKENIZATION
	// Convert split structure to standard argument array for processing
	args = split_to_args(split);
	if (!args)
		return (NULL);    // Memory allocation failure in tokenization
	
	// PHASE 2: ARGUMENT ANALYSIS (First Pass)
	// Count clean arguments to determine allocation size for result array
	j = 0;   // Clean argument counter
	i = 0;   // Argument iterator
	
	while (args[i])
	{
		// REDIRECTION DETECTION: Check current token for redirection operators
		redirect_type = is_redirection(args[i]);
		int numbered_redirect = 0;  // Flag for numbered redirection patterns
		
		// NUMBERED REDIRECTION CHECK: Look for patterns like "2 >"
		// Only check if current token isn't already a redirection operator
		if (!redirect_type && args[i + 1])
			numbered_redirect = is_numbered_redirection(args[i], args[i + 1]);
		
		// SKIP LOGIC: Handle redirection tokens and their targets
		if (redirect_type || numbered_redirect)
		{
			// Skip redirection operator(s) and filename
			// Numbered redirections span 3 tokens: number + operator + filename
			// Regular redirections span 2 tokens: operator + filename
			int skip_count = (numbered_redirect) ? 3 : 2;
			
			// Bounds check: ensure we don't go beyond array bounds
			// If there aren't enough tokens, we'll handle the error in the second pass
			while (skip_count > 0 && args[i])
			{
				i++;
				skip_count--;
			}
		}
		else
		{
			// CLEAN ARGUMENT: Count non-redirection tokens
			j++;    // Increment clean argument count
			i++;    // Move to next token
		}
	}
	
	// PHASE 3: CLEAN ARGUMENT ARRAY ALLOCATION
	// Allocate array sized for clean arguments plus NULL terminator
	clean_args = malloc(sizeof(char *) * (j + 1));
	if (!clean_args)
	{
		free_args(args);    // Clean up original arguments on allocation failure
		return (NULL);
	}
	
	// PHASE 4: REDIRECTION PROCESSING (Second Pass)
	// Process redirections and build clean argument array simultaneously
	i = 0;   // Reset argument iterator
	j = 0;   // Reset clean argument index
	
	while (args[i])
	{
		// REDIRECTION DETECTION: Re-examine tokens for redirection patterns
		redirect_type = is_redirection(args[i]);
		int numbered_redirect = 0;
		
		// NUMBERED REDIRECTION CHECK: Look for multi-token patterns
		if (!redirect_type && args[i + 1])
			numbered_redirect = is_numbered_redirection(args[i], args[i + 1]);
		
		// REDIRECTION PROCESSING: Handle redirection setup and file operations
		if (redirect_type || numbered_redirect)
		{
			// PATTERN UNIFICATION: Use numbered redirect type if detected
			if (numbered_redirect)
				redirect_type = numbered_redirect;
			
			// FILENAME EXTRACTION: Get target filename for redirection
			// Numbered redirections: filename is at position i+2 (num + op + filename)
			// Regular redirections: filename is at position i+1 (op + filename)
			int filename_index = (numbered_redirect) ? i + 2 : i + 1;
			
			// SYNTAX VALIDATION: Ensure filename exists
			if (!args[filename_index])
			{
				write(STDERR_FILENO, "syntax error: missing filename\n", 32);
				free_args(args);        // Clean up all allocated memory
				free(clean_args);
				return (NULL);          // Indicate syntax error
			}
			
			// QUOTE PROCESSING: Remove quotes from filename before file operations
			char *processed_filename = remove_quotes_for_redirection(args[filename_index]);
			if (!processed_filename)
			{
				processed_filename = args[filename_index];  // Fallback to original
			}
			
			// REDIRECTION TYPE DISPATCH: Process based on operator type
			if (redirect_type == 1) // Here document <<
			{
				// HERE DOCUMENT PROCESSING: Set up pipe for inline input
				if (handle_here_doc(processed_filename, here_doc_pipe) == 0)
					*input_fd = here_doc_pipe[0];  // Use pipe read end for stdin
			}
			else if (redirect_type == 2) // Append >>
			{
				// APPEND OUTPUT: Open file in append mode to preserve content
				*output_fd = open(processed_filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
				if (*output_fd == -1)
				{
					perror(processed_filename);  // Report file operation error
					redirection_failed = 1;      // Mark redirection as failed
					// CLEANUP: Free processed filename before breaking
					if (processed_filename && processed_filename != args[filename_index])
						free(processed_filename);
					break;  // Stop processing further redirections
				}
			}
			else if (redirect_type == 3) // Input <
			{
				// INPUT REDIRECTION: Open file for reading
				*input_fd = open(processed_filename, O_RDONLY);
				if (*input_fd == -1)
				{
					perror(processed_filename);  // Report file access error
					redirection_failed = 1;      // Mark redirection as failed
					// CLEANUP: Free processed filename before breaking
					if (processed_filename && processed_filename != args[filename_index])
						free(processed_filename);
					break;  // Stop processing further redirections
				}
			}
			else if (redirect_type == 4) // Output >
			{
				// OUTPUT REDIRECTION: Open file for writing (truncate existing)
				*output_fd = open(processed_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if (*output_fd == -1)
				{
					perror(processed_filename);  // Report file operation error
					redirection_failed = 1;      // Mark redirection as failed
					// CLEANUP: Free processed filename before breaking
					if (processed_filename && processed_filename != args[filename_index])
						free(processed_filename);
					break;  // Stop processing further redirections
				}
			}
			else if (redirect_type == 5) // Error redirection 2>
			{
				// ERROR REDIRECTION: Open file for stderr output
				*stderr_fd = open(processed_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if (*stderr_fd == -1)
				{
					perror(processed_filename);  // Report file operation error
					redirection_failed = 1;      // Mark redirection as failed
					// CLEANUP: Free processed filename before breaking
					if (processed_filename && processed_filename != args[filename_index])
						free(processed_filename);
					break;  // Stop processing further redirections
				}
			}
			
			// CLEANUP: Free processed filename if it was allocated
			if (processed_filename && processed_filename != args[filename_index])
				free(processed_filename);
			
			// SKIP PROCESSED TOKENS: Move past redirection syntax
			int skip_count = (numbered_redirect) ? 3 : 2;
			
			// Bounds check: ensure we don't go beyond array bounds
			while (skip_count > 0 && args[i])
			{
				i++;
				skip_count--;
			}
		}
		else
		{
			// CLEAN ARGUMENT PROCESSING: Copy non-redirection arguments
			clean_args[j] = ft_strdup(args[i]);  // Duplicate argument string
			j++;    // Move to next clean argument slot
			i++;    // Move to next source argument
		}
	}
	
	// ARRAY TERMINATION: Add NULL terminator for execve() compatibility
	clean_args[j] = NULL;
	
	// CHECK FOR REDIRECTION FAILURES: Return error if any redirection failed
	if (redirection_failed)
	{
		free_args(args);
		free_args(clean_args);
		redirection_fail_procedure(input_fd, output_fd, stderr_fd);
		return (NULL);
	}
	
	// CLEANUP AND RETURN: Free original arguments and return clean array
	free_args(args);
	return (clean_args);
}

int	execute_with_redirections(t_split split, t_shell *shell)
{
	char	**args;
	int		input_fd = STDIN_FILENO;
	int		output_fd = STDOUT_FILENO;
	int		stderr_fd = STDERR_FILENO;
	int		saved_stdin = -1;
	int		saved_stdout = -1;
	int		saved_stderr = -1;
	int		exit_status;
	int		i;

	// Parse redirections and get clean arguments
	args = parse_redirections(split, &input_fd, &output_fd, &stderr_fd);
	if (!args)
		return (1);
	
	// If no command after processing redirections
	if (!args[0])
	{
		free_args(args);
		if (input_fd != STDIN_FILENO)
			close(input_fd);
		if (output_fd != STDOUT_FILENO)
			close(output_fd);
		if (stderr_fd != STDERR_FILENO)
			close(stderr_fd);
		return (0);
	}
	
	// Expand environment variables
	i = 0;
	while (args[i])
	{
		char *expanded = expand_variables(args[i], shell);
		if (expanded)
		{
			free(args[i]);
			args[i] = expanded;
		}
		i++;
	}
	
	// Process quotes after variable expansion
	process_args_quotes(args, shell);
	
	// Compact arguments to remove empty strings from variable expansion
	compact_args(args);
	
	// Check for empty command after variable expansion and compaction
	if (!args[0])
	{
		// Close redirected file descriptors before returning
		if (input_fd != STDIN_FILENO)
			close(input_fd);
		if (output_fd != STDOUT_FILENO)
			close(output_fd);
		if (stderr_fd != STDERR_FILENO)
			close(stderr_fd);
		free_args(args);
		return (0);
	}
	
	// For built-in commands, redirect in current process
	if (is_builtin(args[0]))
	{
		// Save original stdin/stdout/stderr
		if (input_fd != STDIN_FILENO)
		{
			saved_stdin = dup(STDIN_FILENO);
			dup2(input_fd, STDIN_FILENO);
			close(input_fd);
		}
		if (output_fd != STDOUT_FILENO)
		{
			saved_stdout = dup(STDOUT_FILENO);
			dup2(output_fd, STDOUT_FILENO);
			close(output_fd);
		}
		if (stderr_fd != STDERR_FILENO)
		{
			saved_stderr = dup(STDERR_FILENO);
			dup2(stderr_fd, STDERR_FILENO);
			close(stderr_fd);
		}
		
		exit_status = execute_builtin(args, shell);
		
		// Restore original stdin/stdout/stderr
		if (saved_stdin != -1)
		{
			dup2(saved_stdin, STDIN_FILENO);
			close(saved_stdin);
		}
		if (saved_stdout != -1)
		{
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
		if (saved_stderr != -1)
		{
			dup2(saved_stderr, STDERR_FILENO);
			close(saved_stderr);
		}
	}
	else
	{
		// For external commands, handle in child process
		char *executable = find_executable(args[0], shell);
		if (!executable)
		{
			write(STDERR_FILENO, args[0], ft_strlen(args[0]));
			write(STDERR_FILENO, ": command not found\n", 20);
			free_args(args);
			return (127);
		}
		
		pid_t pid = fork();
		if (pid == 0)
		{
			// Child process
			setup_child_signals();
			
			if (input_fd != STDIN_FILENO)
			{
				dup2(input_fd, STDIN_FILENO);
				close(input_fd);
			}
			if (output_fd != STDOUT_FILENO)
			{
				dup2(output_fd, STDOUT_FILENO);
				close(output_fd);
			}
			if (stderr_fd != STDERR_FILENO)
			{
				dup2(stderr_fd, STDERR_FILENO);
				close(stderr_fd);
			}
			
			execve(executable, args, shell->envp);
			perror("execve");
			exit(127);
		}
		else
		{
			// Parent process
			int status;
			if (input_fd != STDIN_FILENO)
				close(input_fd);
			if (output_fd != STDOUT_FILENO)
				close(output_fd);
			if (stderr_fd != STDERR_FILENO)
				close(stderr_fd);
			
			waitpid(pid, &status, 0);
			if (WIFEXITED(status))
				exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				exit_status = 128 + WTERMSIG(status);
			else
				exit_status = 1;
		}
		free(executable);
	}
	
	free_args(args);
	return (exit_status);
}
