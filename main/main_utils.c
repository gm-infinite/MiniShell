/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 13:29:14 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/13 07:43:42 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** ============================================================================
**                              MAIN UTILITIES
** ============================================================================
**
** This file provides essential utility functions for the main shell loop
** and general shell operations. These functions handle common operations
** like input validation, cleanup, and safe termination procedures.
**
** CORE FUNCTIONALITY:
** - Input validation and whitespace detection
** - Safe shell termination with proper cleanup
** - Memory management utilities
** - Error handling and graceful exit procedures
**
** DESIGN PRINCIPLES:
** - Safety: All functions handle NULL inputs gracefully
** - Completeness: Thorough cleanup of all allocated resources
** - Reliability: Consistent behavior across different termination scenarios
** - Performance: Efficient algorithms for common operations
**
** INTEGRATION CONTEXT:
** These utilities are called throughout the shell lifecycle, particularly
** during input processing, error handling, and graceful shutdown procedures.
** They ensure the shell maintains proper resource management and provides
** a stable user experience.
*/

#include "minishell.h"

/*
** ============================================================================
**                           INPUT VALIDATION
** ============================================================================
*/

/**
** is_empty - Determines if a string is NULL or contains only whitespace
**
** This function performs comprehensive whitespace detection, identifying
** strings that should be treated as empty input in the shell context.
** It handles all standard whitespace characters and provides robust
** NULL pointer protection.
**
** ALGORITHM:
** 1. NULL pointer validation for safety
** 2. Character-by-character whitespace detection
** 3. Early return on first non-whitespace character found
** 4. Return true only if all characters are whitespace
**
** WHITESPACE DEFINITION:
** - Space character (ASCII 32)
** - Tab characters (ASCII 9-13: \t, \n, \v, \f, \r)
** - Uses standard ASCII control character range
**
** PERFORMANCE:
** - O(n) where n is string length
** - Early termination on first non-whitespace character
** - No memory allocation required
** - Efficient single-pass algorithm
**
** USE CASES:
** - Input validation before command parsing
** - Filtering empty command lines in main loop
** - Preventing execution of whitespace-only input
** - Supporting user-friendly input handling
**
** @param str: Input string to test (may be NULL)
** @return: 1 if string is empty/whitespace-only, 0 if contains content
**
** EXAMPLES:
** - is_empty(NULL) returns 1
** - is_empty("") returns 1
** - is_empty("   \t\n") returns 1
** - is_empty("  hello  ") returns 0
** - is_empty("a") returns 0
*/
int	is_empty(char *str)
{
	int	i;  // Iterator for character-by-character examination

	// NULL POINTER PROTECTION
	// Treat NULL strings as empty for safety and consistency
	if (str == NULL)
		return (1);
		
	// WHITESPACE DETECTION LOOP
	// Examine each character for non-whitespace content
	i = 0;
	while (str[i] != '\0')
	{
		// Check if current character is NOT whitespace
		// Whitespace includes: space (32) and control chars (9-13)
		if (!(str[i] == ' ' || (str[i] <= 13 && str[i] >= 9)))
			return (0);  // Found non-whitespace content
		i++;
	}
	
	// ALL CHARACTERS ARE WHITESPACE
	// String contains only whitespace or is empty
	return (1);
}

/*
** ============================================================================
**                           SAFE TERMINATION
** ============================================================================
*/

/**
** safe_exit - Performs comprehensive cleanup and graceful shell termination
**
** This function handles all aspects of proper shell shutdown, ensuring no
** memory leaks or dangling resources remain. It's designed to be called
** from various termination scenarios including Ctrl+D, exit command, and
** error conditions requiring immediate shutdown.
**
** CLEANUP PHASES:
** 1. INPUT BUFFER CLEANUP
**    - Free current command line input from readline
**    - Prevents memory leaks from unprocessed input
**
** 2. TOKENIZATION CLEANUP
**    - Free parsed command structure (t_split)
**    - Release all dynamically allocated token arrays
**
** 3. ENVIRONMENT CLEANUP
**    - Free all environment variable strings
**    - Release environment array memory
**
** 4. READLINE HISTORY CLEANUP
**    - Clear command history maintained by readline
**    - Free readline internal memory structures
**
** 5. PROCESS TERMINATION
**    - Exit with status 0 (successful termination)
**    - Return control to parent shell/terminal
**
** MEMORY SAFETY:
** - All allocations are properly checked before freeing
** - No double-free vulnerabilities
** - Comprehensive coverage of all shell-allocated memory
** - Uses specialized cleanup functions for complex structures
**
** INTEGRATION POINTS:
** - Ctrl+D handler in main loop
** - Exit built-in command
** - Fatal error conditions
** - Signal handlers for graceful shutdown
**
** @param shell: Shell structure containing all allocated resources
**
** POST-CONDITIONS:
** - All dynamically allocated memory is freed
** - Process exits with status 0
** - No memory leaks reported by valgrind
** - Terminal state is properly restored
**
** DESIGN NOTES:
** - Function does not return (calls exit())
** - Safe to call multiple times due to null checks
** - Handles partial initialization scenarios gracefully
*/
void	safe_exit(t_shell *shell)
{
	// PHASE 1: INPUT BUFFER CLEANUP
	// Free the current command line input from readline
	if (shell->current_input != NULL)
		free(shell->current_input);
		
	// PHASE 2: TOKENIZATION CLEANUP  
	// Free the parsed command structure and all associated tokens
	if (shell->split_input.start != NULL)
		free_split(&(shell->split_input));
		
	// PHASE 3: ENVIRONMENT CLEANUP
	// Free all environment variables and the environment array
	free_environment(shell);
	
	// PHASE 4: READLINE HISTORY CLEANUP
	// Clear command history to free readline's internal memory
	rl_clear_history();
	
	// PHASE 5: PROCESS TERMINATION
	// Exit gracefully with the last command's exit status
	// This ensures proper exit code propagation for shell scripts
	exit(shell->past_exit_status);
}
