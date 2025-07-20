/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe_command.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 22:24:01 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/19 22:24:32 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

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
		char **filtered_args = filter_empty_args(args);
		if (!filtered_args)
		{
			perror("filter_empty_args");
			exit(127);
		}
		if (execve(executable, filtered_args, shell->envp) == -1)
		{
			// EXEC FAILURE: Command exists but cannot be executed
			perror("execve");
			free_args(filtered_args);
			exit(127);
		}
		// execve() only returns on failure - successful exec never returns
	}
	
	// PARENT PROCESS: Clean up resources and return child PID
	free(executable);   // Free path string allocated by find_executable
	free_args(args);    // Free argument array
	return (pid);       // Return child PID for pipeline process tracking
}