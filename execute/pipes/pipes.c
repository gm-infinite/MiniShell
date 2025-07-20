/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 15:24:56 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/19 22:25:34 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

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
	// Process parentheses in the entire command before splitting by pipes
	split = process_parentheses_in_split(split, shell);
	
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
		// Process parentheses in each command before execution
		commands[i] = process_parentheses_in_split(commands[i], shell);
		
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
	
	// PHASE 6: PARENT SIGNAL HANDLING SETUP
	// Ignore SIGINT and SIGQUIT in parent while children execute
	// This prevents the shell from being interrupted by Ctrl+C during pipeline execution
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	
	// PHASE 7: PROCESS SYNCHRONIZATION
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
	
	// PHASE 8: RESTORE SIGNAL HANDLERS
	// Restore normal interactive signal handlers after pipeline completion
	setup_signals();
	
	// PHASE 9: RESOURCE CLEANUP
	// Free all dynamically allocated memory and close remaining descriptors
	for (i = 0; i < cmd_count - 1; i++)
		free(pipes[i]);   // Free individual pipe pairs
	free(pipes);          // Free pipe array
	free(pids);           // Free process ID array
	free(commands);       // Free command segment array
	
	// Return final pipeline exit status
	return (exit_status);
}
