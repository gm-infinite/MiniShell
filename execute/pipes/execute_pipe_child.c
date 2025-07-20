/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe_child.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 22:25:13 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 15:07:01 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

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
		// Redirect stdin to read from the previous command's output
		// pipes[cmd_index-1][0] is the read end of the previous pipe
		dup2(pipes[cmd_index - 1][0], STDIN_FILENO);
	// First command (cmd_index == 0) keeps original stdin (no redirection)
	
	// OUTPUT REDIRECTION: Connect stdout to appropriate data destination
	if (cmd_index < cmd_count - 1) // Not last command: write to next pipe
		// Redirect stdout to write to the next command's input
		// pipes[cmd_index][1] is the write end of the current pipe
		dup2(pipes[cmd_index][1], STDOUT_FILENO);
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
	
	// PHASE 4: PARENTHESES SUBSHELL HANDLING
	// Check if the command contains parentheses - if so, create a subshell
	// and delegate to the AND/OR parser for complex expression handling
	if (has_parentheses_in_split(cmd))
	{
		// Command contains parentheses - process with AND/OR parser in subshell
		// This creates a recursive execution environment for complex expressions
		parser_and_or(shell, cmd);
		exit(shell->past_exit_status);  // Exit child with subshell's exit status
	}

	// PHASE 5: COMMAND TOKENIZATION
	// Convert command segment into executable argument array
	args = split_to_args(cmd);
	if (!args || !args[0])
		// TOKENIZATION FAILURE: Exit with error
		// No cleanup needed as process exit handles memory
		exit(1);
	
	// PHASE 6: ENVIRONMENT VARIABLE EXPANSION
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
	compact_args(args);
	process_args_quotes(args, shell);
	
	// PHASE 7.5: EMPTY COMMAND VALIDATION
	// Check if command became empty after variable expansion and compaction
	if (!args[0])
		exit(0);
	if (is_builtin(args[0]))
		// BUILT-IN COMMAND EXECUTION
		// Execute shell built-in and exit with its return code
		// Built-ins run in child process context within pipelines
		exit(execute_builtin(args, shell));
	else
	{
		// EXTERNAL COMMAND EXECUTION
		
		// PATH RESOLUTION: Find executable in system PATH
		executable = find_executable(args[0], shell);
		if (!executable)
		{
			write(STDERR_FILENO, args[0], ft_strlen(args[0]));
			write(STDERR_FILENO, ": command not found\n", 20);
			exit(127);
		}
		
		// PROCESS REPLACEMENT: Replace child with target command
		char **filtered_args = filter_empty_args(args);
		if (!filtered_args)
		{
			perror("filter_empty_args");
			exit(127);
		}
		execve(executable, filtered_args, shell->envp);
		perror("execve");
		free_args(filtered_args);
		exit(127);
	}
	// This point should never be reached in normal execution
	// execve() replaces the process image, and exit() terminates the process
}
