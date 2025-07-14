/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 00:00:00 by user              #+#    #+#             */
/*   Updated: 2025/07/13 09:28:59 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** COUNT_PIPES - Pipeline Counting Function
** 
** PURPOSE:
** This function performs a critical preprocessing step for pipeline execution by counting
** the number of pipe operators ('|') in a command sequence. This count is essential for:
** 1. Determining how many separate processes need to be created
** 2. Allocating the correct number of pipe file descriptor pairs
** 3. Setting up the proper pipeline execution strategy
** 4. Memory allocation for process tracking arrays
**
** ALGORITHM:
** - Iterate through all tokens in the split structure
** - Compare each token against the pipe operator using ft_strncmp with length 2
** - Increment counter for each pipe found
** - Return total count for downstream pipeline setup
**
** PARAMETERS:
** @split: t_split structure containing tokenized command line
**         - split.start: Array of string pointers to tokens
**         - split.size: Number of tokens in the array
**
** RETURNS:
** Integer representing the number of pipe operators found
** Note: Number of commands = pipe_count + 1 (pipes separate commands)
**
** COMPLEXITY: O(n) where n is the number of tokens
** MEMORY: O(1) - no dynamic allocation, uses only local variables
*/
int	count_pipes(t_split split)
{
	int	i;           // Token iterator - tracks current position in token array
	int	pipe_count;  // Accumulator for pipe operators found

	// Initialize counters for clean state
	pipe_count = 0;
	i = 0;
	
	// Traverse entire token array to count pipe operators
	// Each iteration examines one token for pipe operator match
	while (i < split.size)
	{
		// Use ft_strncmp with length 2 to ensure exact match of "|" operator
		// This prevents false positives from tokens containing "|" as substring
		if (ft_strncmp(split.start[i], "|", 2) == 0)
			pipe_count++;  // Increment counter when pipe operator found
		i++;  // Advance to next token for continued scanning
	}
	
	// Return total count - this will be used to determine:
	// - Number of pipe() system calls needed (pipe_count)
	// - Number of child processes to fork (pipe_count + 1)
	// - Size of arrays for tracking process IDs and pipe file descriptors
	return (pipe_count);
}

/*
** SPLIT_BY_PIPES - Pipeline Command Segmentation Function
**
** PURPOSE:
** This function performs the critical task of breaking down a complex command line
** containing pipe operators into individual command segments. Each segment represents
** a discrete command that will be executed in its own process within the pipeline.
** This segmentation is fundamental to the pipeline execution model where each command
** runs concurrently and data flows from one command to the next via pipe buffers.
**
** ALGORITHM BREAKDOWN:
** 1. PIPE COUNTING: First determines the number of pipe operators present
** 2. MEMORY ALLOCATION: Allocates array to hold individual command segments
** 3. SEGMENTATION: Iterates through tokens, creating new segments at each pipe
** 4. BOUNDARY DETECTION: Establishes start and length for each command segment
**
** MEMORY MANAGEMENT STRATEGY:
** - Allocates single array of t_split structures (one per command)
** - Each t_split points back to original token array (no token duplication)
** - Caller responsible for freeing the returned commands array
** - Original tokens remain owned by input split structure
**
** EDGE CASES HANDLED:
** - No pipes present: Returns single command containing entire input
** - Multiple consecutive pipes: Each creates separate (potentially empty) command
** - Leading/trailing pipes: May create empty commands at boundaries
**
** PARAMETERS:
** @split: Input command line tokens containing pipe-separated commands
** @cmd_count: Output parameter - receives number of command segments created
**
** RETURNS:
** Pointer to array of t_split structures, each representing one command segment
** NULL on memory allocation failure
**
** COMPLEXITY: O(n) where n is number of tokens in input
** MEMORY: O(c) where c is number of commands (minimal overhead)
*/
t_split	*split_by_pipes(t_split split, int *cmd_count)
{
	t_split	*commands;    // Array of command segments to return
	int		pipe_count;   // Number of pipe operators found
	int		i;            // Token iterator for traversing input tokens
	int		cmd_idx;      // Current command index being populated
	int		start;        // Start index of current command segment

	// PHASE 1: ANALYSIS - Count pipes to determine memory requirements
	pipe_count = count_pipes(split);
	*cmd_count = pipe_count + 1;  // Commands = pipes + 1 (pipes separate commands)
	
	// OPTIMIZATION: Handle simple case of no pipes efficiently
	// Avoids unnecessary allocation and copying for single commands
	if (pipe_count == 0)
	{
		commands = malloc(sizeof(t_split));
		if (!commands)
			return (NULL);  // Memory allocation failure
		commands[0] = split;  // Direct assignment - no token copying needed
		return (commands);
	}
	
	// PHASE 2: ALLOCATION - Create array to hold all command segments
	commands = malloc(sizeof(t_split) * (*cmd_count));
	if (!commands)
		return (NULL);  // Critical memory allocation failure
	
	// PHASE 3: SEGMENTATION - Split tokens at pipe boundaries
	cmd_idx = 0;   // Track which command we're currently building
	start = 0;     // Mark beginning of current command's tokens
	i = 0;         // Token position iterator
	
	// Traverse all tokens plus one extra iteration to handle final command
	// The extra iteration (i <= split.size) ensures the last command segment
	// is properly created even when there's no trailing pipe
	while (i <= split.size)
	{
		// BOUNDARY DETECTION: Check if we've hit a pipe or reached the end
		// This creates a command segment from accumulated tokens
		if (i == split.size || (ft_strncmp(split.start[i], "|", 2) == 0))
		{
			// SEGMENT CREATION: Build command from tokens between boundaries
			commands[cmd_idx].start = &split.start[start];  // Point to first token
			commands[cmd_idx].size = i - start;             // Calculate segment length
			cmd_idx++;      // Move to next command slot
			start = i + 1;  // Next command starts after the pipe token
		}
		i++;  // Advance to next token
	}
	
	// Return array of command segments ready for pipeline execution
	// Each segment contains pointers back to original tokens (efficient)
	return (commands);
}

/*
** EXECUTE_PIPE_COMMAND - Individual Pipeline Component Executor
**
** PURPOSE:
** This function handles the execution of a single command within a pipeline context.
** It manages the complex orchestration of process creation, file descriptor redirection,
** environment variable expansion, and command dispatch. This function is designed to
** work within the broader pipeline execution framework where multiple commands run
** concurrently with data flowing between them via pipes.
**
** EXECUTION FLOW:
** 1. INPUT VALIDATION: Ensures command is valid and non-empty
** 2. TOKENIZATION: Converts command segment to executable argument array
** 3. VARIABLE EXPANSION: Resolves environment variables in arguments
** 4. COMMAND CLASSIFICATION: Determines if command is built-in or external
** 5. PROCESS CREATION: Forks child process for command execution
** 6. I/O REDIRECTION: Sets up stdin/stdout redirection via pipes
** 7. COMMAND EXECUTION: Dispatches to appropriate execution handler
** 8. CLEANUP: Manages memory and file descriptor cleanup
**
** BUILT-IN vs EXTERNAL COMMAND HANDLING:
** - Built-ins require special handling in pipelines because they need to run
**   in a separate process to maintain pipeline semantics and I/O redirection
** - External commands follow standard fork/exec pattern with PATH resolution
** - Both types require proper file descriptor management and cleanup
**
** FILE DESCRIPTOR MANAGEMENT:
** This function operates within a complex file descriptor environment:
** - input_fd: Source of data for this command (stdin or previous pipe read end)
** - output_fd: Destination for command output (stdout or next pipe write end)
** - all_pipes: Array of all pipeline pipe pairs for proper cleanup
** - Child processes must close all pipe descriptors to prevent deadlocks
**
** PARAMETERS:
** @cmd: Command segment containing tokens for this pipeline component
** @input_fd: File descriptor for command input (STDIN_FILENO or pipe read end)
** @output_fd: File descriptor for command output (STDOUT_FILENO or pipe write end)
** @shell: Shell state containing environment, settings, and context
** @all_pipes: Array of all pipe pairs in pipeline for cleanup purposes
** @pipe_count: Number of pipe pairs in the complete pipeline
**
** RETURNS:
** Process ID of forked child (positive value) on success
** -1 on failure (fork error, command not found, etc.)
**
** MEMORY SAFETY:
** - All allocated memory is properly freed before return
** - File descriptors are closed appropriately in both parent and child
** - Error paths include proper cleanup to prevent resource leaks
**
** SIGNAL HANDLING:
** Child processes have signal handling configured via setup_child_signals()
** to ensure proper pipeline behavior under signal conditions
*/
int	execute_pipe_command(t_split cmd, int input_fd, int output_fd, t_shell *shell, int **all_pipes, int pipe_count)
{
	char	**args;        // Argument array for command execution
	char	*executable;   // Full path to executable for external commands
	pid_t	pid;          // Process ID of forked child process
	int		i;            // Iterator for various loops (expansion, cleanup)

	// INPUT VALIDATION: Ensure we have a valid command to execute
	// Empty commands can occur from malformed input or parsing edge cases
	if (cmd.size == 0)
		return (1);  // Return non-zero to indicate error condition
		
	// TOKENIZATION: Convert command segment into null-terminated argument array
	// This transforms our internal t_split representation into the standard
	// char** format expected by execve() and built-in command handlers
	args = split_to_args(cmd);
	if (!args)
		return (1);  // Memory allocation failure in tokenization
	
	// VARIABLE EXPANSION PHASE: Process environment variables in all arguments
	// This critical step resolves $VAR references to their actual values
	// Must be done before command classification to handle cases like:
	// export CMD=echo && $CMD "hello" (where $CMD resolves to command name)
	i = 0;
	while (args[i])
	{
		// Expand each argument individually and replace if expansion occurred
		char *expanded = expand_variables(args[i], shell);
		if (expanded)
		{
			free(args[i]);      // Free original argument
			args[i] = expanded; // Replace with expanded version
		}
		i++;  // Continue to next argument
	}
	
	// BUILT-IN COMMAND HANDLING: Special pipeline processing for shell built-ins
	// Built-ins must run in a child process when in a pipeline to maintain
	// proper I/O redirection and pipeline semantics. Running built-ins in the
	// parent process would corrupt the shell's state and break redirection.
	
	// Check for empty command after variable expansion
	if (!args[0] || args[0][0] == '\0')
	{
		free_args(args);
		return (0);  // Empty command is successful no-op
	}
	
	if (is_builtin(args[0]))
	{
		// Fork child process specifically for built-in execution in pipeline
		pid = fork();
		if (pid == 0)
		{
			// CHILD PROCESS: Set up I/O redirection and execute built-in
			
			// INPUT REDIRECTION: Connect command input to appropriate source
			if (input_fd != STDIN_FILENO)
			{
				dup2(input_fd, STDIN_FILENO);  // Redirect stdin to pipe or file
				close(input_fd);               // Close original descriptor
			}
			
			// OUTPUT REDIRECTION: Connect command output to appropriate destination
			if (output_fd != STDOUT_FILENO)
			{
				dup2(output_fd, STDOUT_FILENO);  // Redirect stdout to pipe or file
				close(output_fd);                // Close original descriptor
			}
			
			// PIPE CLEANUP: Close all pipeline file descriptors in child
			// This is critical to prevent deadlocks and resource leaks
			// Child processes must not hold references to pipes they don't use
			for (i = 0; i < pipe_count; i++)
			{
				close(all_pipes[i][0]);  // Close read end of each pipe
				close(all_pipes[i][1]);  // Close write end of each pipe
			}
			
			// Execute built-in command and exit with its return code
			// Built-ins handle their own argument processing and output
			exit(execute_builtin(args, shell));
		}
		
		// PARENT PROCESS: Clean up and return child PID
		free_args(args);  // Free argument array (child has its own copy)
		return (pid);     // Return child PID for process tracking
	}
	
	// EXTERNAL COMMAND HANDLING: Standard fork/exec pattern for system commands
	
	// PATH RESOLUTION: Find executable in system PATH or use absolute path
	executable = find_executable(args[0], shell);
	if (!executable)
	{
		// COMMAND NOT FOUND: Report error and clean up
		write(STDERR_FILENO, args[0], ft_strlen(args[0]));
		write(STDERR_FILENO, ": command not found\n", 20);
		free_args(args);
		return (-1);  // Indicate command resolution failure
	}
	
	// PROCESS CREATION: Fork child process for external command execution
	pid = fork();
	if (pid == -1)
	{
		// FORK FAILURE: Critical system error - clean up and report
		perror("fork");
		free(executable);
		free_args(args);
		return (-1);
	}
	else if (pid == 0)
	{
		// CHILD PROCESS: Set up execution environment and run command
		
		// SIGNAL CONFIGURATION: Set up child-appropriate signal handling
		// Child processes have different signal semantics than the shell
		setup_child_signals();
		
		// I/O REDIRECTION: Connect command to pipeline data flow
		
		// INPUT SETUP: Connect stdin to data source (pipe or file)
		if (input_fd != STDIN_FILENO)
		{
			dup2(input_fd, STDIN_FILENO);  // Redirect input
			close(input_fd);               // Close original descriptor
		}
		
		// OUTPUT SETUP: Connect stdout to data destination (pipe or file)
		if (output_fd != STDOUT_FILENO)
		{
			dup2(output_fd, STDOUT_FILENO);  // Redirect output
			close(output_fd);                // Close original descriptor
		}
		
		// PIPELINE CLEANUP: Close all pipe descriptors in child process
		// Essential for preventing deadlocks and ensuring proper data flow
		for (i = 0; i < pipe_count; i++)
		{
			close(all_pipes[i][0]);  // Close read end
			close(all_pipes[i][1]);  // Close write end
		}
		
		// COMMAND EXECUTION: Replace child process with target command
		if (execve(executable, args, shell->envp) == -1)
		{
			// EXEC FAILURE: Command exists but cannot be executed
			perror("execve");
			exit(127);  // Standard exit code for exec failure
		}
		// execve() only returns on failure - successful exec never returns
	}
	
	// PARENT PROCESS: Clean up resources and return child PID
	free(executable);   // Free path string allocated by find_executable
	free_args(args);    // Free argument array
	return (pid);       // Return child PID for pipeline process tracking
}

/*
** EXECUTE_PIPELINE - Comprehensive Pipeline Execution Engine
**
** PURPOSE:
** This function orchestrates the complete execution of a command pipeline, handling
** everything from single command optimization to complex multi-process coordination.
** It represents the heart of the shell's concurrent execution model, managing process
** creation, inter-process communication via pipes, resource cleanup, and exit status
** propagation across the entire pipeline.
**
** PIPELINE EXECUTION MODEL:
** The function implements a sophisticated concurrent execution model where:
** 1. Multiple processes run simultaneously (one per command)
** 2. Data flows left-to-right through pipe buffers
** 3. Each process has independent stdin/stdout/stderr
** 4. Parent process coordinates and synchronizes all children
** 5. Exit status comes from the last command in the pipeline
**
** ARCHITECTURE OVERVIEW:
** ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐
** │ cmd1    │────│ pipe1   │────│ cmd2    │────│ pipe2   │────│ cmd3    │
** │ PID:100 │    │ [r][w]  │    │ PID:101 │    │ [r][w]  │    │ PID:102 │
** └─────────┘    └─────────┘    └─────────┘    └─────────┘    └─────────┘
**     │                               │                               │
**     └─────────────── Parent waits for all children ────────────────┘
**
** OPTIMIZATION STRATEGIES:
** - Single command: Bypasses pipeline machinery for efficiency
** - Memory pre-allocation: Allocates all required structures upfront
** - Concurrent execution: All commands start simultaneously for maximum throughput
** - Lazy cleanup: Defers resource cleanup until all processes complete
**
** ERROR HANDLING PHILOSOPHY:
** The function employs comprehensive error handling with graceful degradation:
** - Memory allocation failures result in clean abort with resource cleanup
** - Individual command failures don't abort entire pipeline
** - Pipe creation failures abort pipeline with proper cleanup
** - Fork failures are handled gracefully with process tracking
**
** PARAMETERS:
** @split: Complete command line containing pipe-separated commands
** @shell: Shell execution context with environment and state
**
** RETURNS:
** Exit status of pipeline (typically from last command)
** Non-zero values indicate various failure conditions
**
** COMPLEXITY: O(n) where n is number of commands in pipeline
** MEMORY: O(n) for process tracking and pipe management structures
*/
int	execute_pipeline(t_split split, t_shell *shell)
{
	t_split	*commands;      // Array of individual command segments
	int		cmd_count;      // Number of commands in pipeline
	int		**pipes;        // 2D array of pipe file descriptor pairs
	pid_t	*pids;         // Array of child process IDs for tracking
	int		i;             // General iterator for loops
	int		status;        // Wait status from child processes
	int		exit_status = 0; // Final exit code to return

	// PHASE 1: COMMAND SEGMENTATION
	// Break down the complete command line into individual executable segments
	// This separates "cmd1 | cmd2 | cmd3" into ["cmd1", "cmd2", "cmd3"]
	commands = split_by_pipes(split, &cmd_count);
	if (!commands)
		return (1);  // Critical failure in command parsing
	
	// OPTIMIZATION: Single Command Fast Path
	// When no pipes are present, bypass the entire pipeline machinery
	// and execute as a simple command for maximum efficiency
	if (cmd_count == 1)
	{
		char **args = split_to_args(commands[0]);
		if (!args)
		{
			free(commands);
			return (1);  // Tokenization failure
		}
		
		// Execute single command using optimized path
		exit_status = execute_single_command(args, shell);
		
		// Clean up and return result
		free_args(args);
		free(commands);
		return (exit_status);
	}
	
	// PHASE 2: PIPE INFRASTRUCTURE CREATION
	// Allocate and initialize all pipe pairs needed for inter-process communication
	// Pipeline with N commands requires (N-1) pipe pairs
	
	// Allocate array of pipe pair pointers
	pipes = malloc(sizeof(int *) * (cmd_count - 1));
	if (!pipes)
	{
		free(commands);
		return (1);  // Memory allocation failure
	}
	
	// Create all pipe pairs with full error handling
	for (i = 0; i < cmd_count - 1; i++)
	{
		// Allocate space for one pipe pair (read_fd, write_fd)
		pipes[i] = malloc(sizeof(int) * 2);
		if (pipe(pipes[i]) == -1)
		{
			// PIPE CREATION FAILURE: Clean up already created pipes
			perror("pipe");
			
			// Clean up successfully created pipes before this failure
			while (--i >= 0)
			{
				close(pipes[i][0]);  // Close read end
				close(pipes[i][1]);  // Close write end
				free(pipes[i]);      // Free pipe pair memory
			}
			free(pipes);    // Free pipe array
			free(commands); // Free command array
			return (1);     // Indicate pipeline creation failure
		}
	}
	
	// PHASE 3: PROCESS TRACKING SETUP
	// Allocate array to track all child process IDs for synchronization
	pids = malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
	{
		// MEMORY ALLOCATION FAILURE: Clean up all created pipes
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
	
	// PHASE 4: CONCURRENT PROCESS CREATION
	// Create all child processes simultaneously for maximum pipeline throughput
	// Each child will set up its own I/O redirection and execute its command
	for (i = 0; i < cmd_count; i++)
	{
		// Fork child process for each command in pipeline
		pids[i] = fork();
		if (pids[i] == -1)
		{
			// FORK FAILURE: Log error but continue with remaining commands
			perror("fork");
			exit_status = 1;  // Mark pipeline as failed
			break;            // Stop creating additional processes
		}
		else if (pids[i] == 0)
		{
			// CHILD PROCESS: Execute individual command in pipeline context
			// This function handles all I/O redirection and command execution
			execute_pipe_child(commands[i], i, pipes, cmd_count, shell);
			exit(127); // Should never reach here - indicates execution failure
		}
		// PARENT PROCESS: Continue to next command creation
	}
	
	// PHASE 5: PARENT PROCESS CLEANUP
	// Close all pipe file descriptors in parent process
	// This is critical for proper pipeline operation:
	// - Prevents parent from holding extra references to pipes
	// - Allows EOF propagation when writers close
	// - Prevents deadlocks in complex pipeline scenarios
	for (i = 0; i < cmd_count - 1; i++)
	{
		close(pipes[i][0]);  // Close read end in parent
		close(pipes[i][1]);  // Close write end in parent
	}
	
	// PHASE 6: PROCESS SYNCHRONIZATION
	// Wait for all child processes to complete and collect exit statuses
	// The exit status of the pipeline is determined by the last command
	for (i = 0; i < cmd_count; i++)
	{
		if (pids[i] > 0)  // Only wait for successfully forked children
		{
			// Wait for specific child and collect its exit status
			waitpid(pids[i], &status, 0);
			
			// LAST COMMAND DETERMINES PIPELINE EXIT STATUS
			// This matches bash behavior where pipeline exit status
			// comes from the rightmost command in the pipeline
			if (i == cmd_count - 1) // Last command in pipeline
			{
				// Extract exit status from wait status
				if (WIFEXITED(status))
					exit_status = WEXITSTATUS(status);  // Normal exit
				else if (WIFSIGNALED(status))
					exit_status = 128 + WTERMSIG(status);  // Signal termination
			}
		}
	}
	
	// PHASE 7: RESOURCE CLEANUP
	// Free all dynamically allocated memory and close remaining descriptors
	for (i = 0; i < cmd_count - 1; i++)
		free(pipes[i]);   // Free individual pipe pairs
	free(pipes);          // Free pipe array
	free(pids);           // Free process ID array
	free(commands);       // Free command segment array
	
	// Return final pipeline exit status
	return (exit_status);
}

/*
** EXECUTE_PIPE_CHILD - Pipeline Child Process Execution Handler
**
** PURPOSE:
** This function runs exclusively in child processes within a pipeline context.
** It handles the complete setup and execution of a single command within the
** larger pipeline framework. The function is responsible for establishing the
** correct I/O redirection, cleaning up inherited file descriptors, processing
** command arguments, and ultimately executing the target command.
**
** EXECUTION CONTEXT:
** This function runs in a forked child process, meaning:
** - It has its own memory space (copy-on-write from parent)
** - Changes to variables don't affect the parent process
** - It inherits all file descriptors from the parent
** - It must clean up inherited resources it doesn't need
** - It should exit (not return) when complete
**
** I/O REDIRECTION STRATEGY:
** The function implements a sophisticated I/O redirection scheme:
** 
** For First Command (cmd_index == 0):
**   stdin:  No redirection (reads from terminal/input)
**   stdout: Redirected to pipes[0][1] (write end of first pipe)
**
** For Middle Commands (0 < cmd_index < cmd_count-1):
**   stdin:  Redirected from pipes[cmd_index-1][0] (read end of previous pipe)
**   stdout: Redirected to pipes[cmd_index][1] (write end of next pipe)
**
** For Last Command (cmd_index == cmd_count-1):
**   stdin:  Redirected from pipes[cmd_index-1][0] (read end of previous pipe)
**   stdout: No redirection (writes to terminal/output)
**
** ARGUMENT PROCESSING:
** The function performs comprehensive argument processing:
** 1. Tokenization: Converts command segment to argument array
** 2. Variable Expansion: Resolves environment variables with quote awareness
** 3. Quote Processing: Handles single and double quote semantics
** 4. Null Termination: Ensures proper array termination for exec functions
**
** PARAMETERS:
** @cmd: Command segment containing tokens for this pipeline component
** @cmd_index: Position of this command in pipeline (0-based)
** @pipes: Array of all pipeline pipe pairs for I/O setup
** @cmd_count: Total number of commands in pipeline
** @shell: Shell execution context with environment and state
**
** RETURNS:
** This function never returns - it either:
** - Successfully executes via execve() (process replacement)
** - Exits with error code on failure
**
** CRITICAL DESIGN NOTES:
** - All pipe descriptors must be closed to prevent deadlocks
** - Signal handling must be configured for child processes
** - Memory allocated in child is not freed (process exit cleans up)
** - Exit codes follow standard Unix conventions (127 = command not found)
*/
void	execute_pipe_child(t_split cmd, int cmd_index, int **pipes, int cmd_count, t_shell *shell)
{
	char	**args;        // Null-terminated argument array for execution
	char	*executable;   // Full path to executable for external commands
	int		i;            // Iterator for loops (expansion, cleanup)

	// PHASE 1: I/O REDIRECTION SETUP
	// Configure stdin and stdout based on position in pipeline
	// This creates the data flow chain that defines pipeline behavior
	
	// INPUT REDIRECTION: Connect stdin to appropriate data source
	if (cmd_index > 0) // Not first command: read from previous pipe
	{
		// Redirect stdin to read from the previous command's output
		// pipes[cmd_index-1][0] is the read end of the previous pipe
		dup2(pipes[cmd_index - 1][0], STDIN_FILENO);
	}
	// First command (cmd_index == 0) keeps original stdin (no redirection)
	
	// OUTPUT REDIRECTION: Connect stdout to appropriate data destination
	if (cmd_index < cmd_count - 1) // Not last command: write to next pipe
	{
		// Redirect stdout to write to the next command's input
		// pipes[cmd_index][1] is the write end of the current pipe
		dup2(pipes[cmd_index][1], STDOUT_FILENO);
	}
	// Last command keeps original stdout (no redirection)
	
	// PHASE 2: FILE DESCRIPTOR CLEANUP
	// Close all inherited pipe descriptors to prevent deadlocks and resource leaks
	// Child processes inherit all file descriptors from parent, but only need
	// stdin/stdout which have been redirected above
	for (i = 0; i < cmd_count - 1; i++)
	{
		close(pipes[i][0]);  // Close read end of each pipe
		close(pipes[i][1]);  // Close write end of each pipe
	}
	// After this cleanup, the child only has stdin/stdout/stderr
	
	// PHASE 3: SIGNAL CONFIGURATION
	// Set up child-appropriate signal handling behavior
	// Child processes should handle signals differently than the interactive shell
	setup_child_signals();
	
	// PHASE 4: COMMAND TOKENIZATION
	// Convert command segment into executable argument array
	args = split_to_args(cmd);
	if (!args || !args[0])
	{
		// TOKENIZATION FAILURE: Exit with error
		// No cleanup needed as process exit handles memory
		exit(1);
	}
	
	// PHASE 5: ENVIRONMENT VARIABLE EXPANSION
	// Process all arguments to resolve $VAR references to actual values
	// This must be done with quote awareness to preserve shell semantics
	i = 0;
	while (args[i])
	{
		// Expand variables with quote context preservation
		char *expanded = expand_variables_quoted(args[i], shell);
		if (expanded)
		{
			free(args[i]);      // Free original argument
			args[i] = expanded; // Replace with expanded version
		}
		i++;  // Continue to next argument
	}
	
	// PHASE 6: QUOTE PROCESSING
	// Handle quote removal and escape sequence processing
	// This step processes quotes that weren't handled during expansion
	process_args_quotes(args, shell);
	
	// Compact arguments to remove empty strings from variable expansion
	compact_args(args);
	
	// PHASE 6.5: EMPTY COMMAND VALIDATION
	// Check if command became empty after variable expansion and compaction
	if (!args[0])
		exit(0);  // Empty command is successful no-op
	
	// PHASE 7: COMMAND EXECUTION DISPATCH
	// Determine command type and execute using appropriate mechanism
	
	if (is_builtin(args[0]))
	{
		// BUILT-IN COMMAND EXECUTION
		// Execute shell built-in and exit with its return code
		// Built-ins run in child process context within pipelines
		exit(execute_builtin(args, shell));
	}
	else
	{
		// EXTERNAL COMMAND EXECUTION
		
		// PATH RESOLUTION: Find executable in system PATH
		executable = find_executable(args[0], shell);
		if (!executable)
		{
			// COMMAND NOT FOUND: Report error and exit
			write(STDERR_FILENO, args[0], ft_strlen(args[0]));
			write(STDERR_FILENO, ": command not found\n", 20);
			exit(127);  // Standard exit code for command not found
		}
		
		// PROCESS REPLACEMENT: Replace child with target command
		execve(executable, args, shell->envp);
		
		// EXEC FAILURE: execve() only returns on failure
		perror("execve");
		exit(127);  // Standard exit code for execution failure
	}
	
	// This point should never be reached in normal execution
	// execve() replaces the process image, and exit() terminates the process
}
