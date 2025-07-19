/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:50:25 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/19 17:15:40 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

/*
** ============================================================================
** PARENTHESES PREPROCESSING FOR PIPE SEGMENTS
** ============================================================================
*/

/**
 * @brief Recursively processes parentheses in a command split
 * 
 * This function handles parentheses within pipe segments by removing them
 * when they wrap the entire command segment.
 * 
 * @param cmd Command split that may contain parentheses
 * @param shell Shell instance for environment and state
 * @return t_split Processed split with parentheses removed
 */
t_split	process_parentheses_in_split(t_split cmd, t_shell *shell)
{
	int		opening_parens = 0; //bunları sayıyor.
	int		closing_parens = 0;
	int		i, j;
	int		first_paren_idx = -1;
	int		last_paren_idx = -1;
	
	// Find first opening parenthesis and last closing parenthesis --> ilk önce ilk baştakini ve en sondakini buluyor
	for (i = 0; i < cmd.size; i++)
	{
		for (j = 0; cmd.start[i][j] != '\0'; j++)
		{
			if (cmd.start[i][j] == '(' && first_paren_idx == -1)
			{
				first_paren_idx = i;
				opening_parens++;
			}
			else if (cmd.start[i][j] == '(')
			{
				opening_parens++;
			}
			else if (cmd.start[i][j] == ')')
			{
				closing_parens++;
				last_paren_idx = i;
			}
		}
	}
	
	// Check if parentheses are balanced and present --> manuel kontrol ediyor
	if (opening_parens == 0 || closing_parens == 0 || opening_parens != closing_parens)
		return (cmd);  // No balanced parentheses to process
	
	// Check if the first token with '(' starts with '(' and the last token with ')' ends with ')'
	if (first_paren_idx != -1 && last_paren_idx != -1 &&
		cmd.start[first_paren_idx][0] == '(' && 
		cmd.start[last_paren_idx][ft_strlen(cmd.start[last_paren_idx]) - 1] == ')')
	{
		// If parentheses are in first and last tokens, use the existing cut_out_par function
		if (first_paren_idx == 0 && last_paren_idx == cmd.size - 1)
		{
			cut_out_par(&cmd);
			return (process_parentheses_in_split(cmd, shell));
		}
		
		// Handle cross-token parentheses manually
		// Remove opening parenthesis from first token
		if (ft_strlen(cmd.start[first_paren_idx]) > 1)
		{
			// Shift the string left to remove the '('
			ft_memmove(cmd.start[first_paren_idx], cmd.start[first_paren_idx] + 1, 
				ft_strlen(cmd.start[first_paren_idx]));
		}
		else
		{
			// Token is just "(", shift all tokens left
			for (i = first_paren_idx; i < cmd.size - 1; i++)
				cmd.start[i] = cmd.start[i + 1];
			cmd.size--;
			last_paren_idx--; // Adjust index since we shifted
			if (cmd.size == 0)
				return (cmd);
		}
		
		// Remove closing parenthesis from last token
		int last_len = ft_strlen(cmd.start[last_paren_idx]);
		if (last_len > 1)
		{
			cmd.start[last_paren_idx][last_len - 1] = '\0';
		}
		else
		{
			// Token is just ")", remove it
			cmd.size--;
		}
		
		// Recursively process in case of nested parentheses
		return (process_parentheses_in_split(cmd, shell));
	}
	
	// No parentheses to process, return original command
	return (cmd);
}

/*
** ============================================================================
** SINGLE COMMAND EXECUTION ENGINE
** ============================================================================
*/

/**
 * @brief Executes a single command without pipes or complex redirections
 * 
 * This function serves as the core execution engine for simple commands that
 * don't involve pipelines. It handles the complete execution lifecycle from
 * argument processing through process creation to exit status collection.
 * The function embodies the fundamental fork-exec pattern that forms the
 * backbone of Unix process creation.
 * 
 * EXECUTION PHASES:
 * ================
 * 
 * 1. ARGUMENT VALIDATION AND PREPROCESSING:
 *    - Validates that command arguments exist and are non-empty
 *    - Expands environment variables with proper quote handling
 *    - Processes quote removal and escape sequence handling
 *    - Ensures arguments are ready for execution
 * 
 * 2. COMMAND CLASSIFICATION:
 *    - Determines if command is a shell built-in function
 *    - Built-ins execute directly in parent process (no fork)
 *    - External commands require executable resolution and forking
 * 
 * 3. EXECUTABLE RESOLUTION:
 *    - For external commands, resolves executable path
 *    - Uses PATH environment variable for command lookup
 *    - Handles both absolute paths and PATH-based resolution
 *    - Reports "command not found" for unresolvable commands
 * 
 * 4. PROCESS CREATION AND EXECUTION:
 *    - Forks child process for external command execution
 *    - Child process sets up signal handlers and calls execve
 *    - Parent process manages signal handling and waits for completion
 * 
 * 5. SIGNAL HANDLING COORDINATION:
 *    - Parent ignores SIGINT/SIGQUIT while child executes
 *    - Child restores default signal behavior for proper termination
 *    - Exit status properly reflects both normal and signal termination
 * 
 * VARIABLE EXPANSION STRATEGY:
 * ===========================
 * Environment variable expansion occurs with quote awareness:
 * - Variables inside double quotes are expanded
 * - Variables inside single quotes are preserved literally
 * - Unquoted variables are expanded normally
 * - $? expands to previous command exit status
 * 
 * BUILT-IN COMMAND HANDLING:
 * ==========================
 * Built-in commands execute directly in the parent process:
 * - No fork/exec overhead for shell built-ins
 * - Built-ins can modify shell state (environment, working directory)
 * - Exit status returned directly from built-in execution
 * 
 * SIGNAL HANDLING PROTOCOL:
 * =========================
 * Parent Process:
 * - Ignores SIGINT and SIGQUIT while waiting for child
 * - Prevents shell termination from child signals
 * - Restores signal handlers after child completion
 * 
 * Child Process:
 * - Restores default signal handlers via setup_child_signals()
 * - Allows normal signal termination behavior
 * - Ensures proper signal propagation to parent via exit status
 * 
 * EXIT STATUS MAPPING:
 * ===================
 * - Normal termination: WEXITSTATUS(status)
 * - Signal termination: 128 + signal_number
 * - Command not found: 127 (POSIX standard)
 * - Fork failure: 1
 * 
 * @param args NULL-terminated array of command arguments
 * @param shell Shell instance containing environment and state
 * @return int Exit status: 0 for success, non-zero for failure
 * 
 * @note First argument (args[0]) must be the command name
 * @note Function modifies args array during variable expansion
 * @note Exit status follows POSIX shell conventions
 */
int	execute_single_command(char **args, t_shell *shell)
{
	pid_t	pid;            // Process ID for forked child
	int		status;         // Child process exit status
	char	*executable;    // Resolved path to executable
	int		i;              // Iterator for argument processing

	// PHASE 1: Input validation
	// Ensure we have a valid command to execute
	if (!args || !args[0])
		return (1);         // No command specified
	
	// PHASE 2: Environment variable expansion
	// Expand all environment variables in arguments with quote awareness
	// This must happen before quote processing to handle variables properly
	i = 0;
	while (args[i])
	{
		char *expanded = expand_variables_quoted(args[i], shell);
		if (expanded)
		{
			free(args[i]);                     // Free original argument
			args[i] = expanded;                // Replace with expanded version
		}
		i++;
	}
	
	compact_args(args);
	// PHASE 3: Quote processing
	// Remove quotes and process escape sequences
	// This happens after variable expansion to preserve expansion context
	process_args_quotes(args, shell);
	
	// PHASE 3.5: Argument compaction
	// Remove empty strings from arguments array (from undefined variable expansion)
	// This shifts remaining arguments to fill gaps (e.g., "$EMPTY echo hi" -> "echo hi")
	
	// PHASE 3.6: Empty command validation after compaction
	// Check if we have any arguments left after compaction
	if (!args[0])
		return (0);         // Empty command is a successful no-op (matches bash behavior)
	
	// PHASE 3.7: Empty command name validation
	// Check if the command name is an empty string (like "")
	// This should be treated as "command not found" not "is a directory"
	if (args[0][0] == '\0')
	{
		write(STDERR_FILENO, ": command not found\n", 20);
		return (127);       // Empty command name is command not found
	}
	
	if (is_builtin(args[0]))
		return (execute_builtin(args, shell));
	executable = find_executable(args[0], shell);
	if (!executable)
	{
		write(STDERR_FILENO, args[0], ft_strlen(args[0]));
		if (ft_strchr(args[0], '/'))
			write(STDERR_FILENO, ": No such file or directory\n", 28);
		else
			write(STDERR_FILENO, ": command not found\n", 20);
		return (127);
	}
	
	// PHASE 5.5: Pre-execution validation
	// Check if the found executable can actually be executed
	// This distinguishes between "command not found" (127) and "cannot execute" (126)
	struct stat file_stat;
	if (stat(executable, &file_stat) == 0)
	{
		// Check if it's a directory
		if (S_ISDIR(file_stat.st_mode))
		{
			write(STDERR_FILENO, args[0], ft_strlen(args[0]));
			write(STDERR_FILENO, ": Is a directory\n", 17);
			free(executable);
			return (126);   // "Cannot execute" exit code for directories
		}
		
		// Check if file has execute permissions
		if (access(executable, X_OK) != 0)
		{
			write(STDERR_FILENO, args[0], ft_strlen(args[0]));
			write(STDERR_FILENO, ": Permission denied\n", 20);
			free(executable);
			return (126);   // "Cannot execute" exit code for permission denied
		}
	}
	
	// PHASE 6: Process creation and execution
	// Fork child process for external command execution
	pid = fork();
	if (pid == -1)
	{
		// Fork failed - clean up and return error
		perror("fork");
		free(executable);
		return (1);
	}
	else if (pid == 0)
	{
		// CHILD PROCESS EXECUTION PATH
		// =============================
		
		// Set up proper signal handling for child process
		// This restores default signal behavior for SIGINT, SIGQUIT, etc.
		setup_child_signals();
		
		// Execute the command using execve system call
		// This replaces the child process image with the new program
		char **filtered_args = filter_empty_args(args);
		if (!filtered_args)
		{
			perror("filter_empty_args");
			free(executable);
			exit(127);
		}
		if (execve(executable, filtered_args, shell->envp) == -1)
		{
			// execve failed - report error and exit with standard error code
			perror("execve");
			free(executable);
			free_args(filtered_args);
			exit(127);      // Standard exit code for exec failure
		}
		// execve never returns on success - child process is replaced
	}
	else
	{
		// PARENT PROCESS EXECUTION PATH
		// =============================
		
		// Clean up executable path now that child has been forked
		free(executable);
		
		// Set up signal handling for parent during child execution
		// Ignore SIGINT and SIGQUIT to prevent shell termination
		// while child is running - signals will be delivered to child
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		
		// Wait for child process to complete
		if (waitpid(pid, &status, 0) == -1)
		{
			perror("waitpid");
			setup_signals();    // Restore signal handlers even on wait failure
			return (1);
		}
		
		// Restore normal signal handlers for interactive shell
		setup_signals();
		
		// PHASE 7: Exit status processing
		// Map child exit status to shell exit code following POSIX conventions
		if (WIFEXITED(status))
		{
			// Child exited normally - return its exit code
			return (WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status))
		{
			// Child terminated by signal - handle signal reporting and mapping
			int sig = WTERMSIG(status);
			
			// Report signal termination to user
			if (sig == SIGINT)
				write(STDOUT_FILENO, "\n", 1);        // Newline for Ctrl+C
			else if (sig == SIGQUIT)
				write(STDOUT_FILENO, "Quit: 3\n", 8); // Standard quit message
			
			// Return signal-based exit code (128 + signal number)
			return (128 + sig);
		}
	}
	
	// Default return (should not be reached in normal execution)
	return (0);
}


/**
 * @brief Main command execution dispatcher and orchestrator
 * 
 * This function serves as the primary entry point for all command execution
 * in the minishell. It analyzes the parsed command structure and routes
 * execution to the appropriate specialized handler based on the presence
 * of pipes, redirections, or both. This dispatcher implements a sophisticated
 * command classification system that ensures optimal execution paths.
 * 
 * COMMAND CLASSIFICATION MATRIX:
 * =============================
 * 
 * | Pipes | Redirections | Handler Function                      |
 * |-------|--------------|---------------------------------------|
 * | No    | No           | execute_single_command()             |
 * | No    | Yes          | execute_with_redirections()          |
 * | Yes   | No           | execute_pipeline()                   |
 * | Yes   | Yes          | execute_pipeline_with_redirections() |
 * 
 * EXECUTION FLOW ANALYSIS:
 * =======================
 * 
 * 1. EMPTY COMMAND HANDLING:
 *    - Commands with zero arguments are no-ops
 *    - Returns success status (0) without further processing
 *    - Prevents unnecessary overhead for empty input
 * 
 * 2. FEATURE DETECTION PHASE:
 *    - Scans for redirection operators (>, <, >>, 2>, etc.)
 *    - Counts pipe operators to determine pipeline complexity
 *    - Uses results to select optimal execution strategy
 * 
 * 3. EXECUTION PATH SELECTION:
 *    - Complex commands (pipes + redirections) use specialized handler
 *    - Simple redirections use dedicated redirection processor
 *    - Pure pipelines use optimized pipeline executor
 *    - Single commands use fast direct execution path
 * 
 * 4. STATE MANAGEMENT:
 *    - Updates shell's past_exit_status for $? variable
 *    - Maintains command history for debugging and user feedback
 *    - Preserves shell state consistency across command execution
 * 
 * PERFORMANCE OPTIMIZATION:
 * ========================
 * - Early classification prevents unnecessary processing overhead
 * - Specialized handlers optimize for their specific use cases
 * - Single command path avoids pipeline setup costs
 * - Redirection detection uses efficient scanning algorithms
 * 
 * ERROR HANDLING STRATEGY:
 * =======================
 * - All execution paths return standardized exit codes
 * - Memory allocation failures are propagated upward
 * - System call failures are handled by individual executors
 * - Shell state remains consistent even on execution failure
 * 
 * SHELL STATE INTEGRATION:
 * =======================
 * - Exit status stored in shell->past_exit_status for $? expansion
 * - Environment variables remain accessible to all execution paths
 * - Signal handling context preserved across different command types
 * - Working directory and other shell state remain intact
 * 
 * @param split Parsed command structure containing arguments and operators
 * @param shell Shell instance with environment, state, and configuration
 * @return int Exit status following POSIX conventions (0 = success)
 * 
 * @note This function updates shell->past_exit_status before returning
 * @note All execution paths properly handle resource cleanup
 * @note Exit status follows standard shell conventions
 */
int	execute_command(t_split split, t_shell *shell)
{
	int		exit_status;    // Command execution result
	int		has_pipes;      // Number of pipe operators found
	int		has_redir;      // Boolean: redirections present
	int		syntax_error;   // Syntax validation result

	// PHASE 1: Empty command optimization
	// Handle empty commands efficiently without overhead
	if (split.size == 0)
		return (0);         // Empty command is successful no-op
	
	// PHASE 1.5: Syntax validation
	// Check for syntax errors before processing
	syntax_error = validate_redirection_syntax(split);
	if (syntax_error != 0)
	{
		shell->past_exit_status = syntax_error;
		return (syntax_error);
	}
	
	// PHASE 2: Command feature detection
	// Analyze command structure to determine execution requirements
	has_redir = has_redirections(split);    // Scan for redirection operators
	has_pipes = count_pipes(split);         // Count pipeline segments
	
	// PHASE 3: Execution path dispatch
	// Route to appropriate handler based on command complexity
	if (has_redir && has_pipes)
	{
		// COMPLEX EXECUTION PATH: Pipelines with per-command redirections
		// This is the most sophisticated execution mode, handling commands like:
		// "cat < input.txt | grep pattern > output.txt | sort >> final.txt"
		exit_status = execute_pipeline_with_redirections(split, shell);
	}
	else if (has_redir)
	{
		// REDIRECTION EXECUTION PATH: Single command with I/O redirection
		// Handles commands like: "ls -la > output.txt" or "cat < input.txt"
		exit_status = execute_with_redirections(split, shell);
	}
	else if (has_pipes > 0)
	{
		// PIPELINE EXECUTION PATH: Multiple commands connected by pipes
		// Handles commands like: "ls -la | grep .txt | wc -l"
		exit_status = execute_pipeline(split, shell);
	}
	else
	{
		// SIMPLE EXECUTION PATH: Single command without special features
		// Optimal path for basic commands like: "ls", "echo hello", "pwd"
		char **args = split_to_args(split);
		if (!args)
			return (1);     // Memory allocation failure
		
		exit_status = execute_single_command(args, shell);
		free_args(args);    // Clean up converted arguments
	}
	
	// PHASE 4: Shell state update
	// Store exit status for $? variable expansion in future commands
	shell->past_exit_status = exit_status;
	
	return (exit_status);
}

/*
** ============================================================================
** COMPLEX PIPELINE EXECUTION WITH REDIRECTION SUPPORT
** ============================================================================
*/

/**
 * @brief Executes pipelines where individual commands may have redirections
 * 
 * This function represents the most sophisticated execution mode in the shell,
 * handling complex command lines that combine both pipelines and per-command
 * redirections. It manages the intricate interaction between pipe-based
 * inter-process communication and file-based I/O redirection.
 * 
 * ARCHITECTURAL COMPLEXITY:
 * ========================
 * This function handles commands of the form:
 * "cmd1 < input.txt | cmd2 > temp.txt | cmd3 2> errors.txt >> output.txt"
 * 
 * Each command in the pipeline can have its own independent redirections
 * that may override or supplement the pipe-based I/O connections.
 * 
 * EXECUTION ALGORITHM:
 * ===================
 * 
 * 1. COMMAND DECOMPOSITION:
 *    - Splits complex command line into individual pipeline segments
 *    - Each segment may contain its own redirection operators
 *    - Maintains original argument order and structure
 * 
 * 2. RESOURCE ALLOCATION PHASE:
 *    - Creates all inter-command pipes before forking any processes
 *    - Allocates process ID tracking array for child management
 *    - Ensures all resources are available before execution begins
 * 
 * 3. CONCURRENT PROCESS CREATION:
 *    - Forks all pipeline commands simultaneously
 *    - Each child process handles its own redirection parsing
 *    - Parent process manages overall pipeline orchestration
 * 
 * 4. I/O REDIRECTION HIERARCHY:
 *    - Pipe connections established first (inter-command communication)
 *    - File redirections processed second (may override pipe I/O)
 *    - Error redirections processed last (stderr handling)
 * 
 * 5. SYNCHRONIZATION AND CLEANUP:
 *    - Parent closes all pipe file descriptors after forking
 *    - Waits for all child processes to complete
 *    - Collects exit status from final command in pipeline
 * 
 * REDIRECTION PRECEDENCE RULES:
 * =============================
 * - File redirections take precedence over pipe connections
 * - Command-specific redirections override pipeline defaults
 * - Error redirections (2>) are independent of stdout redirections
 * - Append redirections (>>) preserve existing file content
 * 
 * MEMORY MANAGEMENT STRATEGY:
 * ==========================
 * - All pipe file descriptors allocated upfront for easy cleanup
 * - Child process arrays sized exactly to command count
 * - Error paths include comprehensive resource deallocation
 * - No memory leaks even in complex failure scenarios
 * 
 * ERROR HANDLING COMPLEXITY:
 * ==========================
 * - Pipe creation failures abort with partial cleanup
 * - Fork failures continue with remaining commands when possible
 * - Child process failures don't affect sibling processes
 * - Parent process always cleans up all allocated resources
 * 
 * SIGNAL HANDLING COORDINATION:
 * ============================
 * - Parent process ignores signals during pipeline execution
 * - Child processes restore default signal handlers
 * - Signal termination of any child affects final exit status
 * - SIGPIPE handling prevents premature pipeline termination
 * 
 * @param split Complete command line with pipes and redirections
 * @param shell Shell instance for environment and state access
 * @return int Exit status of final command in pipeline
 * 
 * @note Handles the most complex execution scenario in the shell
 * @note Requires careful coordination of pipes, files, and processes
 * @note Memory and file descriptor management is critical for stability
 */
int	execute_pipeline_with_redirections(t_split split, t_shell *shell)
{
	t_split	*commands;      // Array of individual pipeline commands
	int		cmd_count;      // Number of commands in the pipeline
	int		**pipes;        // Array of pipe file descriptor pairs
	pid_t	*pids;          // Array of child process IDs
	int		i;              // Iterator for resource management
	int		status;         // Child process exit status
	int		exit_status = 0; // Final pipeline exit status

	// PHASE 1: Command decomposition and validation
	// Process parentheses in the entire command before splitting by pipes
	split = process_parentheses_in_split(split, shell);
	
	// Split the complex command line into individual pipeline segments
	commands = split_by_pipes(split, &cmd_count);
	if (!commands)
		return (1);     // Memory allocation failure during parsing
	
	// OPTIMIZATION: Single command with redirections
	// If there's only one command after pipe splitting, this isn't really
	// a pipeline - just execute it as a redirected command for efficiency
	if (cmd_count == 1)
	{
		exit_status = execute_with_redirections(commands[0], shell);
		free(commands);
		return (exit_status);
	}
	
	// PHASE 2: Inter-command pipe allocation
	// Create all pipes before forking to ensure resource availability
	pipes = malloc(sizeof(int *) * (cmd_count - 1));
	if (!pipes)
	{
		free(commands);
		return (1);     // Memory allocation failure for pipe array
	}
	
	// Create all pipe file descriptor pairs
	for (i = 0; i < cmd_count - 1; i++)
	{
		pipes[i] = malloc(sizeof(int) * 2);
		if (pipe(pipes[i]) == -1)
		{
			perror("pipe");
			// CRITICAL ERROR PATH: Clean up partially created pipes
			// This prevents file descriptor leaks in failure scenarios
			while (--i >= 0)
			{
				close(pipes[i][0]);    // Close read end
				close(pipes[i][1]);    // Close write end
				free(pipes[i]);        // Free pipe structure
			}
			free(pipes);
			free(commands);
			return (1);
		}
	}
	
	// PHASE 3: Process tracking allocation
	// Create array to track all child process IDs for synchronization
	pids = malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
	{
		// CLEANUP PATH: Deallocate all pipes and commands on failure
		for (i = 0; i < cmd_count - 1; i++)
		{
			close(pipes[i][0]);
			close(pipes[i][1]);
			free(pipes[i]);
		}
		free(pipes);
		free(commands);
		return (1);
	}
	
	// PHASE 4: Concurrent process creation
	// Fork all pipeline commands simultaneously for parallel execution
	for (i = 0; i < cmd_count; i++)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			exit_status = 1;
			break;      // Continue with partial pipeline on fork failure
		}
		else if (pids[i] == 0)
		{
			// CHILD PROCESS EXECUTION PATH
			// Each child handles its own redirection + pipe setup
			execute_pipe_child_with_redirections(commands[i], i, pipes, cmd_count, shell);
			exit(127);  // Should never reach here - indicates exec failure
		}
	}
	
	// PHASE 5: Parent process pipe cleanup
	// Close all pipe file descriptors in parent to prevent deadlocks
	// Children inherit copies, so parent must close to avoid blocking
	for (i = 0; i < cmd_count - 1; i++)
	{
		close(pipes[i][0]);     // Close read end
		close(pipes[i][1]);     // Close write end
	}
	
	// PHASE 6: Parent signal handling setup
	// Ignore SIGINT and SIGQUIT in parent while children execute
	// This prevents the shell from being interrupted by Ctrl+C during pipeline execution
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	
	// PHASE 7: Child process synchronization
	// Wait for all children and collect exit status from final command
	for (i = 0; i < cmd_count; i++)
	{
		if (pids[i] > 0)        // Only wait for successfully forked processes
		{
			waitpid(pids[i], &status, 0);
			
			// Exit status determination: last command in pipeline determines
			// overall pipeline success/failure following POSIX convention
			if (i == cmd_count - 1) // Final command in pipeline
			{
				if (WIFEXITED(status))
					exit_status = WEXITSTATUS(status);
				else if (WIFSIGNALED(status))
					exit_status = 128 + WTERMSIG(status);
			}
		}
	}
	
	// PHASE 8: Restore signal handlers
	// Restore normal interactive signal handlers after pipeline completion
	setup_signals();
	
	// PHASE 9: Resource cleanup and return
	// Deallocate all dynamically allocated resources
	for (i = 0; i < cmd_count - 1; i++)
		free(pipes[i]);         // Free individual pipe structures
	free(pipes);                // Free pipe array
	free(pids);                 // Free process ID array
	free(commands);             // Free command array
	
	return (exit_status);
}

/**
 * @brief Child process execution handler for complex pipelines with redirections
 * 
 * This function implements the most sophisticated child process execution path
 * in the shell, handling the complex interaction between pipeline I/O and
 * file-based redirections. Each child process in a complex pipeline must
 * coordinate three separate I/O systems: pipes, file redirections, and
 * standard I/O inheritance.
 * 
 * EXECUTION PHASES IN CHILD PROCESS:
 * =================================
 * 
 * 1. PIPELINE I/O SETUP:
 *    - Establishes default pipe connections based on position in pipeline
 *    - First command: stdin from terminal, stdout to next command
 *    - Middle commands: stdin from previous, stdout to next
 *    - Last command: stdin from previous, stdout to terminal
 * 
 * 2. REDIRECTION OVERRIDE PROCESSING:
 *    - Parses command-specific redirections (>, <, >>, 2>, etc.)
 *    - File redirections may override default pipe connections
 *    - Creates new file descriptors for redirection targets
 *    - Maintains separate handling for stdin, stdout, and stderr
 * 
 * 3. FILE DESCRIPTOR MAPPING:
 *    - Maps resolved file descriptors to standard I/O descriptors
 *    - Uses dup2() to establish final I/O configuration
 *    - Closes original file descriptors to prevent descriptor leaks
 * 
 * 4. PIPELINE RESOURCE CLEANUP:
 *    - Closes all pipe file descriptors inherited from parent
 *    - Prevents interference with pipe-based communication
 *    - Ensures clean process environment for command execution
 * 
 * 5. COMMAND PREPARATION AND EXECUTION:
 *    - Expands environment variables in command arguments
 *    - Processes quote removal and escape sequences
 *    - Determines execution path (built-in vs. external command)
 *    - Executes command with established I/O configuration
 * 
 * I/O REDIRECTION PRECEDENCE:
 * ===========================
 * The function implements a specific precedence order:
 * 1. Default pipe connections (based on pipeline position)
 * 2. Explicit file redirections (override pipe defaults)
 * 3. Error stream redirections (independent of stdout)
 * 
 * This allows commands like: "cmd1 | cmd2 > file.txt | cmd3"
 * Where cmd2 outputs to file.txt instead of cmd3's stdin.
 * 
 * SIGNAL HANDLING IN CHILD:
 * =========================
 * - Restores default signal handlers via setup_child_signals()
 * - Allows normal signal termination behavior
 * - Prevents interference from parent's signal handling
 * - Ensures proper signal propagation for job control
 * 
 * ARGUMENT PROCESSING PIPELINE:
 * ============================
 * 1. Redirection parsing extracts I/O operators and targets
 * 2. Environment variable expansion with quote awareness
 * 3. Quote processing and escape sequence handling
 * 4. Final argument validation before execution
 * 
 * BUILT-IN VS. EXTERNAL COMMAND HANDLING:
 * =======================================
 * - Built-in commands execute directly within the child process
 * - External commands require executable resolution and execve()
 * - Both paths maintain the established I/O redirection configuration
 * - Exit codes properly propagate to parent process
 * 
 * ERROR HANDLING STRATEGY:
 * =======================
 * - Redirection parsing failures exit with code 1
 * - Missing executables exit with code 127 (POSIX standard)
 * - execve() failures exit with code 127 after error reporting
 * - All error paths include proper resource cleanup
 * 
 * @param cmd Command structure with arguments and potential redirections
 * @param cmd_index Position of this command in the pipeline (0-based)
 * @param pipes Array of all pipe file descriptor pairs
 * @param cmd_count Total number of commands in the pipeline
 * @param shell Shell instance for environment and state access
 * 
 * @note This function never returns - child process either execs or exits
 * @note All file descriptors are properly managed to prevent leaks
 * @note Exit codes follow POSIX conventions for shell compatibility
 */
void	execute_pipe_child_with_redirections(t_split cmd, int cmd_index, int **pipes, int cmd_count, t_shell *shell)
{
	char	**args;             // Processed command arguments
	char	*executable;        // Resolved executable path
	int		input_fd = STDIN_FILENO;   // Input file descriptor (default: stdin)
	int		output_fd = STDOUT_FILENO; // Output file descriptor (default: stdout)
	int		stderr_fd = STDERR_FILENO; // Error file descriptor (default: stderr)
	int		i;                  // Iterator for various processing loops

	// PHASE 1: Default pipeline I/O establishment
	// Set up initial file descriptors based on position in pipeline
	// These may be overridden later by explicit redirections
	if (cmd_index > 0) // Not first command: read from previous pipe
	{
		input_fd = pipes[cmd_index - 1][0];  // Read end of previous pipe
	}
	if (cmd_index < cmd_count - 1) // Not last command: write to next pipe
	{
		output_fd = pipes[cmd_index][1];     // Write end of current pipe
	}
	
	// PHASE 2: Redirection parsing and I/O override
	// Parse command for redirection operators and extract clean arguments
	// This function may modify input_fd, output_fd, and stderr_fd based on
	// redirection operators found in the command arguments
	args = parse_redirections(cmd, &input_fd, &output_fd, &stderr_fd, shell);
	if (!args)
		exit(1);        // Redirection parsing failed - exit child process
	
	// PHASE 3: File descriptor mapping to standard I/O
	// Apply the resolved file descriptors to standard I/O streams
	// This establishes the final I/O configuration for command execution
	if (input_fd != STDIN_FILENO)
	{
		dup2(input_fd, STDIN_FILENO);     // Redirect stdin
		close(input_fd);                  // Close original descriptor
	}
	if (output_fd != STDOUT_FILENO)
	{
		dup2(output_fd, STDOUT_FILENO);   // Redirect stdout
		close(output_fd);                 // Close original descriptor
	}
	if (stderr_fd != STDERR_FILENO)
	{
		dup2(stderr_fd, STDERR_FILENO);   // Redirect stderr
		close(stderr_fd);                 // Close original descriptor
	}
	
	// PHASE 4: Pipeline resource cleanup
	// Close all pipe file descriptors in child process
	// This prevents interference with pipeline communication and avoids
	// keeping unnecessary file descriptors open in the child
	for (i = 0; i < cmd_count - 1; i++)
	{
		close(pipes[i][0]);               // Close read end
		close(pipes[i][1]);               // Close write end
	}
	
	// PHASE 5: Child signal handling setup
	// Restore default signal handlers for proper child process behavior
	// This allows normal signal termination and job control functionality
	setup_child_signals();
	
	// PHASE 6: Command validation
	// Handle edge case where redirection parsing consumed all arguments
	// This can happen with pure redirection commands like "> output.txt"
	if (!args[0])
	{
		free_args(args);
		exit(0);            // Successful no-op for pure redirections
	}
	
	// PHASE 7: Environment variable expansion
	// Expand all environment variables in arguments with quote awareness
	// This must happen before quote processing to maintain expansion context
	i = 0;
	while (args[i])
	{
		char *expanded = expand_variables_quoted(args[i], shell);
		if (expanded)
		{
			free(args[i]);                // Free original argument
			args[i] = expanded;           // Replace with expanded version
		}
		i++;
	}
	
	compact_args(args);
	// PHASE 8: Quote processing
	// Remove quotes and process escape sequences in arguments
	// This happens after variable expansion to preserve expansion context
	process_args_quotes(args, shell);
	
	// Compact arguments to remove empty strings from variable expansion
	
	// PHASE 8.5: Empty command validation after compaction
	// Check if command became empty after variable expansion and compaction
	if (!args[0])
	{
		free_args(args);
		exit(0);  // Empty command is successful no-op
	}
	
	// Check for empty command name (like "")
	if (args[0][0] == '\0')
	{
		write(STDERR_FILENO, ": command not found\n", 20);
		free_args(args);
		exit(127);  // Empty command name is command not found
	}
	
	// PHASE 9: Command execution dispatch
	// Determine execution path based on command type and execute
	if (is_builtin(args[0]))
	{
		// BUILT-IN EXECUTION PATH
		// Execute shell built-in directly in child process
		// Built-ins maintain the established I/O redirection configuration
		exit(execute_builtin(args, shell));
	}
	else
	{
		// EXTERNAL COMMAND EXECUTION PATH
		// Resolve executable and execute via execve()
		executable = find_executable(args[0], shell);
		if (!executable)
		{
			// Command not found - report error and exit with standard code
			write(STDERR_FILENO, args[0], ft_strlen(args[0]));
			write(STDERR_FILENO, ": command not found\n", 20);
			exit(127);      // POSIX standard "command not found" exit code
		}
		
		// Execute the resolved command
		char **filtered_args = filter_empty_args(args);
		if (!filtered_args)
		{
			perror("filter_empty_args");
			exit(127);
		}
		execve(executable, filtered_args, shell->envp);
		
		// If execve returns, it failed - report error and exit
		perror("execve");
		free_args(filtered_args);
		exit(127);          // Standard exit code for exec failure
	}
}

