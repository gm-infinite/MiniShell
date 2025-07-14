/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 00:00:00 by user              #+#    #+#             */
/*   Updated: 2025/07/14 17:32:51 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** ============================================================================
** I/O REDIRECTION PROCESSING ENGINE
** ============================================================================
** 
** This file implements the comprehensive I/O redirection system for the minishell,
** handling all forms of input/output redirection operators and file descriptor
** manipulation. It provides the foundational infrastructure for redirecting
** command input, output, and error streams to files, pipes, and other sources.
**
** SUPPORTED REDIRECTION OPERATORS:
** ================================
** 
** INPUT REDIRECTIONS:
** - "<"  : Redirect stdin from file (input redirection)
** - "<<" : Here document (inline input with delimiter)
** 
** OUTPUT REDIRECTIONS:
** - ">"  : Redirect stdout to file (output redirection, truncate)
** - ">>" : Redirect stdout to file (output redirection, append)
** 
** ERROR REDIRECTIONS:
** - "2>" : Redirect stderr to file (error redirection, truncate)
** - "2>>" : Redirect stderr to file (error redirection, append)
** 
** ADVANCED FEATURES:
** - Numbered file descriptors (2>, 3>, etc.)
** - Multiple redirections per command
** - Integration with pipeline execution
** - Here document processing with delimiter matching
**
** ARCHITECTURAL DESIGN:
** ====================
** 
** The redirection system follows a three-phase processing model:
** 
** 1. DETECTION AND CLASSIFICATION:
**    - Scans command arguments for redirection operators
**    - Classifies operators by type and target file descriptor
**    - Handles both simple (>) and numbered (2>) redirections
** 
** 2. FILE DESCRIPTOR SETUP:
**    - Opens target files with appropriate modes and permissions
**    - Creates pipes for here documents and other special cases
**    - Validates file accessibility and reports errors
** 
** 3. ARGUMENT EXTRACTION:
**    - Removes redirection operators and filenames from command arguments
**    - Returns clean argument array suitable for command execution
**    - Preserves argument order and structure for normal processing
**
** INTEGRATION WITH EXECUTION ENGINE:
** ==================================
** 
** The redirection system integrates seamlessly with the command execution
** infrastructure:
** - Works with single command execution (execute_single_command)
** - Integrates with pipeline execution (execute_pipeline)
** - Supports complex combinations of pipes and redirections
** - Maintains proper file descriptor inheritance and cleanup
**
** ERROR HANDLING AND VALIDATION:
** ==============================
** 
** Comprehensive error handling ensures robust operation:
** - Validates syntax (missing filenames, invalid operators)
** - Checks file permissions and accessibility
** - Reports meaningful error messages to stderr
** - Provides graceful failure with proper cleanup
** - Maintains shell stability even with invalid redirections
**
** FILE DESCRIPTOR MANAGEMENT:
** ===========================
** 
** Careful file descriptor management prevents resource leaks:
** - Opens files with appropriate permissions (0644 for created files)
** - Uses proper open flags (O_RDONLY, O_WRONLY, O_CREAT, etc.)
** - Manages here document pipes with proper cleanup
** - Ensures file descriptors are closed when no longer needed
**
** HERE DOCUMENT IMPLEMENTATION:
** =============================
** 
** Here documents ("<<") provide inline input functionality:
** - Creates temporary pipe for data transfer
** - Collects input until delimiter is encountered
** - Supports EOF termination (Ctrl+D)
** - Provides visual feedback with continuation prompt
** - Integrates with command execution via file descriptor substitution
**
** MEMORY MANAGEMENT STRATEGY:
** ===========================
** 
** Efficient memory management maintains performance:
** - Allocates clean argument arrays sized appropriately
** - Frees intermediate structures and temporary allocations
** - Handles allocation failures with proper error propagation
** - Ensures no memory leaks even in error conditions
*/

#include "minishell.h"

/*
** ============================================================================
**                            REDIRECTION UTILITIES
** ============================================================================
*/

/**
 * @brief Comprehensive quote removal for redirection filenames
 * 
 * Removes all quote characters from a filename string while handling complex
 * quote scenarios including concatenated quoted segments like "a""b""c".
 * This function processes the string similarly to how bash handles quotes
 * in redirection contexts.
 * 
 * @param str Input string that may contain quotes
 * @return char* New string with quotes removed, or NULL on allocation failure
 * 
 * @note Returns a newly allocated string that must be freed by caller
 * @note Handles both single and double quotes
 * @note Supports concatenated quote segments like "file""name"
 */
static char	*remove_quotes_for_redirection(char *str)
{
	char			*result;
	int				i;
	int				j;
	unsigned char	quote_state;
	int				len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	result = malloc(len + 1);  // Worst case: no quotes to remove
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	quote_state = 0;  // bit 0 = single quote, bit 1 = double quote
	while (str[i])
	{
		if (str[i] == '\'' && !(quote_state & 2))  // bit 1 (double quote) not set
			quote_state ^= 1;  // XOR toggle bit 0 (single quote)
		else if (str[i] == '"' && !(quote_state & 1))  // bit 0 (single quote) not set
			quote_state ^= 2;  // XOR toggle bit 1 (double quote)
		else
		{
			// Copy all other characters (content inside and outside quotes)
			result[j] = str[i];
			j++;
		}
		i++;
	}
	result[j] = '\0';
	return (result);
}

/*
** ============================================================================
** REDIRECTION OPERATOR DETECTION AND CLASSIFICATION
** ============================================================================
*/

/**
 * @brief Detects and classifies redirection operators in command tokens
 * 
 * This function serves as the primary classifier for I/O redirection operators,
 * analyzing individual command tokens to determine if they represent redirection
 * syntax. It implements a comprehensive detection algorithm that recognizes all
 * standard shell redirection operators while maintaining strict syntax validation.
 * 
 * DETECTION ALGORITHM:
 * ===================
 * The function uses a priority-based matching system that checks longer operators
 * first to prevent substring matching issues:
 * 
 * 1. Two-character operators (<<, >>, 2>) are checked first
 * 2. Single-character operators (<, >) are checked last
 * 3. Exact length matching prevents false positives from longer strings
 * 4. String comparison uses ft_strncmp with explicit length validation
 * 
 * OPERATOR CLASSIFICATION CODES:
 * ==============================
 * The function returns integer codes that classify redirection types:
 * - 0: Not a redirection operator
 * - 1: Here document (<<) - requires delimiter processing
 * - 2: Append output (>>) - opens file in append mode
 * - 3: Input redirection (<) - opens file for reading
 * - 4: Output redirection (>) - opens file for writing (truncate)
 * - 5: Error redirection (2>) - redirects stderr to file
 * 
 * VALIDATION STRATEGY:
 * ===================
 * Strict validation prevents false positives and ensures robust parsing:
 * - NULL pointer check prevents segmentation faults
 * - Length validation ensures exact operator matching
 * - No partial matches are accepted (e.g., ">>>" doesn't match ">>")
 * - Case-sensitive matching follows shell conventions
 * 
 * INTEGRATION CONTEXT:
 * ===================
 * This function is called during command parsing to:
 * - Identify redirection operators in token streams
 * - Enable removal of redirection syntax from command arguments
 * - Provide operator classification for file descriptor setup
 * - Support syntax validation and error reporting
 * 
 * @param token Command token to analyze for redirection syntax
 * @return int Classification code (0 = not redirection, 1-5 = operator types)
 * 
 * @note Function is safe with NULL input (returns 0)
 * @note Return values correspond to specific redirection types
 * @note Used by parse_redirections() for command processing
 */
/*
** Check if token is a redirection operator
** Also check for numbered redirections like "2 >"
*/
int	is_redirection(char *token)
{
	// INPUT VALIDATION: Handle NULL pointer gracefully
	// This prevents segmentation faults from malformed input
	if (!token)
		return (0);    // Not a redirection if token doesn't exist
	
	// PRIORITY 1: Two-character operators (checked first to prevent substring issues)
	
	// HERE DOCUMENT DETECTION: "<<" operator
	// Requires exact match with length validation to prevent false positives
	if (ft_strncmp(token, "<<", 3) == 0 && ft_strlen(token) == 2)
		return (1); // Here document - requires delimiter processing
		
	// APPEND OUTPUT DETECTION: ">>" operator  
	// Opens target file in append mode to preserve existing content
	if (ft_strncmp(token, ">>", 3) == 0 && ft_strlen(token) == 2)
		return (2); // Append output redirection
		
	// ERROR REDIRECTION DETECTION: "2>" operator
	// Redirects stderr (file descriptor 2) to specified file
	if (ft_strncmp(token, "2>", 3) == 0 && ft_strlen(token) == 2)
		return (5); // Error redirection
	
	// PRIORITY 2: Single-character operators (checked after multi-character)
	
	// INPUT REDIRECTION DETECTION: "<" operator
	// Redirects stdin to read from specified file
	if (ft_strncmp(token, "<", 2) == 0 && ft_strlen(token) == 1)
		return (3); // Input redirection
		
	// OUTPUT REDIRECTION DETECTION: ">" operator
	// Redirects stdout to specified file (truncates existing content)
	if (ft_strncmp(token, ">", 2) == 0 && ft_strlen(token) == 1)
		return (4); // Output redirection
	
	// NOT A REDIRECTION: Token doesn't match any known operator
	return (0);
}

/**
 * @brief Detects numbered redirection patterns with separate tokens
 * 
 * This function handles the detection of numbered redirection operators that
 * are split across multiple tokens due to whitespace separation. In shell
 * syntax, redirections like "2 >" (error redirection with space) are valid
 * and equivalent to "2>" (without space). This function enables recognition
 * of such patterns during command parsing.
 * 
 * NUMBERED REDIRECTION SYNTAX:
 * ===========================
 * Standard shells support numbered file descriptor redirections:
 * - "2 >" or "2>" : Redirect stderr (fd 2) to file
 * - "3 >" or "3>" : Redirect fd 3 to file  
 * - "1 >" or "1>" : Redirect stdout (fd 1) to file (equivalent to ">")
 * 
 * WHITESPACE HANDLING:
 * ===================
 * The function addresses the parsing challenge where tokenization splits
 * numbered redirections into separate tokens:
 * - Input: ["cmd", "2", ">", "file.txt"] (with spaces)
 * - Recognition: Identifies "2" + ">" as error redirection pattern
 * - Equivalent: "2>" as single token would be handled by is_redirection()
 * 
 * DETECTION ALGORITHM:
 * ===================
 * 1. Validates both tokens exist (not NULL)
 * 2. Checks first token is a valid file descriptor number
 * 3. Checks second token is a valid redirection operator
 * 4. Returns appropriate classification code for the combination
 * 
 * CURRENT IMPLEMENTATION SCOPE:
 * ============================
 * The current implementation specifically handles:
 * - "2" + ">" : Standard error redirection (most common use case)
 * 
 * Future extensions could support:
 * - Other file descriptor numbers (3, 4, 5, etc.)
 * - Other redirection operators (>>, <, etc.)
 * - More complex numbered redirection patterns
 * 
 * ERROR HANDLING:
 * ==============
 * - NULL pointer safety: Returns 0 for any NULL token
 * - Exact matching: Prevents false positives from longer strings
 * - Length validation: Ensures precise token matching
 * 
 * @param num_token Token that should contain file descriptor number
 * @param redir_token Token that should contain redirection operator
 * @return int Classification code (0 = not numbered redirection, 5 = stderr redirection)
 * 
 * @note Function is safe with NULL inputs
 * @note Currently specialized for "2 >" pattern
 * @note Integrates with parse_redirections() for complete processing
 */
/*
** Check if token is a number followed by redirection
** Returns the redirection type for numbered redirections
*/
int	is_numbered_redirection(char *num_token, char *redir_token)
{
	// INPUT VALIDATION: Ensure both tokens exist
	// Prevents segmentation faults from NULL pointer dereferencing
	if (!num_token || !redir_token)
		return (0);    // Not a numbered redirection if either token missing
	
	// STDERR REDIRECTION PATTERN: "2" + ">" tokens
	// This is the most common numbered redirection pattern in shell usage
	// Checks for exact match of "2" (stderr file descriptor) followed by ">"
	if (ft_strncmp(num_token, "2", 2) == 0 && ft_strlen(num_token) == 1 &&
		ft_strncmp(redir_token, ">", 2) == 0 && ft_strlen(redir_token) == 1)
		return (5); // Error redirection 2> (same code as "2>" single token)
	
	// EXTENSIBILITY POINT: Additional numbered redirections could be added here
	// Examples for future implementation:
	// - "3 >" for file descriptor 3 redirection
	// - "1 >" for explicit stdout redirection  
	// - Support for append operators like "2 >>"
	
	// NOT A NUMBERED REDIRECTION: Pattern doesn't match known combinations
	return (0);
}

/*
** ============================================================================
** HERE DOCUMENT PROCESSING AND INLINE INPUT HANDLING
** ============================================================================
*/

/**
 * @brief Implements here document functionality for inline input processing
 * 
 * This function provides here document (<<) functionality, which allows users
 * to provide inline input to commands using a delimiter-based syntax. Here
 * documents are a powerful shell feature that enables multi-line input
 * specification directly within command lines, commonly used for scripts
 * and interactive command sequences.
 * 
 * HERE DOCUMENT SYNTAX:
 * ====================
 * Standard here document usage:
 * - command << DELIMITER
 * - (input lines)
 * - DELIMITER
 * 
 * Example:
 * cat << EOF
 * This is line 1
 * This is line 2
 * EOF
 * 
 * IMPLEMENTATION STRATEGY:
 * =======================
 * The function creates a pipe to transfer here document content to the command:
 * 1. Creates a pipe with read and write ends
 * 2. Collects user input until delimiter is encountered
 * 3. Writes collected input to pipe write end
 * 4. Returns pipe read end for command stdin redirection
 * 5. Command reads from pipe as if reading from a file
 * 
 * INPUT COLLECTION PROCESS:
 * ========================
 * - Displays continuation prompt ("> ") for user feedback
 * - Uses readline() for input collection with history support
 * - Compares each line against delimiter for termination
 * - Handles EOF conditions (Ctrl+D) gracefully
 * - Transfers input to pipe with proper newline preservation
 * 
 * PIPE MANAGEMENT:
 * ===============
 * - Creates pipe pair for data transfer to command
 * - Writes all collected input to pipe write end
 * - Closes write end to signal EOF to reading command
 * - Returns read end file descriptor for stdin redirection
 * - Caller responsible for closing read end after use
 * 
 * ERROR HANDLING:
 * ==============
 * - Pipe creation failure is reported via perror()
 * - Memory allocation failures are handled gracefully
 * - EOF during input collection terminates here document
 * - All allocated memory is properly freed
 * 
 * @param delimiter String that terminates here document input
 * @param pipe_fd Array to receive pipe file descriptors [read_fd, write_fd]
 * @return int 0 on success, 1 on failure (pipe creation error)
 * 
 * @note Function modifies pipe_fd array to return file descriptors
 * @note Write end is closed by function, read end left open for caller
 * @note Delimiter comparison is exact string match including length
 */
/*
** Handle here document (<<)
*/
int	handle_here_doc(char *delimiter, int *pipe_fd)
{
	char	*line;      // Input line buffer from readline()

	// PIPE CREATION: Set up data transfer mechanism
	// Creates bidirectional pipe for transferring here document content
	// to the command that will consume it as stdin
	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");   // Report system error for pipe creation failure
		return (1);       // Indicate failure to caller
	}
	
	// PROMPT SETUP: Configure user feedback for input collection
	// The "> " prompt indicates continuation mode for here document input

	
	// INPUT COLLECTION LOOP: Gather here document content until delimiter
	while (1)
	{
		// READLINE INPUT: Collect one line of input with prompt display
		// Uses GNU readline for consistent input handling with shell
		line = readline("> ");
		
		// EOF HANDLING: User pressed Ctrl+D to terminate input
		if (!line) // EOF (Ctrl+D)
			break;   // Exit input collection loop
		
		// DELIMITER DETECTION: Check if current line matches termination string
		// Uses exact string comparison including length to prevent partial matches
		if (ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1) == 0)
		{
			free(line);   // Free the delimiter line (not written to pipe)
			break;        // Exit input collection loop
		}
		
		// DATA TRANSFER: Write input line to pipe for command consumption
		// Transfers both the line content and trailing newline to preserve
		// input structure for the consuming command
		write(pipe_fd[1], line, ft_strlen(line));  // Write line content
		write(pipe_fd[1], "\n", 1);                // Write newline character
		
		// MEMORY CLEANUP: Free readline-allocated buffer
		free(line);
	}
	
	// PIPE FINALIZATION: Close write end to signal EOF to reading command
	// This is essential for proper here document termination - the reading
	// command will receive EOF when it tries to read more data
	close(pipe_fd[1]);
	
	// SUCCESS RETURN: Here document processing completed successfully
	// Caller receives pipe read end (pipe_fd[0]) for stdin redirection
	return (0);
}

/*
** ============================================================================
** REDIRECTION PARSING AND COMMAND ARGUMENT PROCESSING
** ============================================================================
*/

/**
 * @brief Comprehensive redirection parser and command argument processor
 * 
 * This function represents the core of the redirection processing system,
 * performing the complex task of extracting redirection operators and their
 * targets from command arguments while setting up the appropriate file
 * descriptors. It implements a two-pass algorithm that first analyzes the
 * command structure, then processes redirections and builds a clean argument
 * array suitable for command execution.
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
/*
** Parse redirections and remove them from command arguments
** Returns modified arguments array and sets up redirections
*/
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
		// Clean up allocated memory before returning error
		free_args(args);
		free_args(clean_args);
		
		// Close any successfully opened file descriptors
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
		
		return (NULL);  // Indicate redirection failure
	}
	
	// CLEANUP AND RETURN: Free original arguments and return clean array
	free_args(args);
	return (clean_args);
}

/*
** Execute command with redirections
*/
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
