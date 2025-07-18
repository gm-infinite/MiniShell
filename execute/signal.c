/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 14:18:07 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/16 20:58:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

/**
 * @brief Primary signal handler for interactive shell mode
 * 
 * This function implements the core signal handling logic for the interactive
 * shell, providing proper terminal behavior when users interrupt operations
 * or attempt to quit. It integrates deeply with the GNU readline library to
 * ensure clean prompt handling and consistent user experience.
 * 
 * SIGNAL PROCESSING STRATEGY:
 * ==========================
 * The handler implements different behaviors for different signals:
 * 
 * SIGINT (Ctrl+C) Processing:
 * - Sets global flag for main loop detection
 * - Outputs newline for clean visual separation
 * - Informs readline that cursor is now on new line
 * - Clears current input buffer to remove partial commands
 * - Redisplays prompt for continued user interaction
 * 
 * SIGQUIT (Ctrl+\) Processing:
 * - Deliberately ignored (no action taken)
 * - Prevents accidental shell termination
 * - Maintains shell stability during interactive use
 * 
 * READLINE INTEGRATION DETAILS:
 * ============================
 * The function uses specific readline functions for proper terminal control:
 * 
 * - rl_on_new_line(): Critical for cursor position tracking after newline
 * - rl_replace_line("", 0): Clears input buffer without affecting history
 * - rl_redisplay(): Forces prompt redraw for immediate visual feedback
 * 
 * This sequence ensures that after Ctrl+C, the user sees a clean new prompt
 * without any artifacts from the interrupted command.
 * 
 * GLOBAL STATE MANAGEMENT:
 * =======================
 * The handler sets g_signal to communicate with the main shell loop:
 * - Enables main loop to detect signal events
 * - Allows proper exit status setting (130 for SIGINT)
 * - Maintains separation between signal context and main execution
 * 
 * SIGNAL SAFETY CONSIDERATIONS:
 * ============================
 * The handler only uses async-signal-safe functions:
 * - write() for output (async-signal-safe)
 * - readline functions (specially designed for signal context)
 * - Simple assignment operations for global variable
 * 
 * @param sig Signal number received (SIGINT, SIGQUIT, etc.)
 * 
 * @note This handler only operates in parent shell process
 * @note Function must be async-signal-safe due to signal context
 * @note Integrates with GNU readline for proper terminal behavior
 */

void	signal_handler(int sig)
{
	// SIGINT PROCESSING: Handle Ctrl+C interruption
	if (sig == SIGINT)
	{
		// GLOBAL STATE UPDATE: Mark signal receipt for main loop
		g_signal = SIGINT;
		
		// VISUAL FEEDBACK: Provide immediate newline for clean separation
		// This ensures the ^C display doesn't interfere with the new prompt
		write(STDOUT_FILENO, "\n", 1);
		
		// READLINE INTEGRATION: Coordinate with readline for proper display
		
		// Inform readline that cursor is now on a new line after the newline
		// This is critical for proper cursor positioning and prompt display
		rl_on_new_line();
		
		// Clear the current input buffer to remove any partial command
		// The empty string and 0 flag clear the line without affecting history
		rl_replace_line("", 0);
		
		// Force redisplay of the prompt for immediate user feedback
		// This shows the new prompt without waiting for user input
		rl_redisplay();
	}
	
	// SIGQUIT PROCESSING: Deliberately ignored
	// Other signals (like SIGQUIT) are intentionally not handled here
	// This prevents accidental shell termination and maintains stability
}

/**
 * @brief Signal handler specifically designed for child processes
 * 
 * This function provides signal handling for child processes (commands
 * executed by the shell), implementing standard Unix signal semantics
 * that differ from the interactive shell behavior. Child processes
 * should respond to signals in the traditional Unix manner rather than
 * the specialized interactive shell behavior.
 * 
 * CHILD PROCESS SIGNAL PHILOSOPHY:
 * ================================
 * Unlike the parent shell, child processes should:
 * - Respond normally to termination signals
 * - Provide feedback about signal-based termination
 * - Allow normal Unix signal propagation
 * - Exit cleanly when signaled
 * 
 * SIGNAL-SPECIFIC BEHAVIOR:
 * ========================
 * 
 * SIGINT (Ctrl+C) in Child Process:
 * - Sets global signal flag for parent notification
 * - Outputs newline for clean terminal appearance
 * - Allows normal signal termination to proceed
 * - Parent will detect child termination and handle appropriately
 * 
 * SIGQUIT (Ctrl+\) in Child Process:
 * - Sets global signal flag for parent notification
 * - Outputs standard quit message with signal number
 * - Follows Unix convention for quit signal reporting
 * - Enables debugging and user feedback for quit operations
 * 
 * COORDINATION WITH PARENT:
 * ========================
 * The child signal handler coordinates with the parent process:
 * - Sets g_signal to inform parent of signal type
 * - Provides visual feedback directly to terminal
 * - Allows signal to continue normal termination process
 * - Parent process will collect exit status and handle cleanup
 * 
 * TERMINAL OUTPUT CONSIDERATIONS:
 * ==============================
 * Direct terminal output in signal handlers requires careful coordination:
 * - Uses write() for async-signal-safe output
 * - Provides immediate feedback without buffering delays
 * - Maintains consistent terminal state across signal events
 * - Ensures user sees appropriate signal termination messages
 * 
 * @param sig Signal number received (SIGINT, SIGQUIT, etc.)
 * 
 * @note This handler only operates in child processes
 * @note Function must be async-signal-safe due to signal context
 * @note Allows normal Unix signal termination after feedback
 */
/*
** Signal handler for child processes
** Let default signal behavior happen for child processes
*/
void	signal_handler_child(int sig)
{
	// SIGINT PROCESSING: Handle Ctrl+C in child process
	if (sig == SIGINT)
	{
		// PARENT NOTIFICATION: Set global flag for parent process visibility
		g_signal = SIGINT;
		
		// TERMINAL FEEDBACK: Provide clean visual separation
		// Ensures terminal appearance is consistent after signal interruption
		write(STDOUT_FILENO, "\n", 1);
	}
	// SIGQUIT PROCESSING: Handle Ctrl+\ in child process
	else if (sig == SIGQUIT)
	{
		// PARENT NOTIFICATION: Set global flag for parent process visibility
		g_signal = SIGQUIT;
		
		// QUIT MESSAGE: Provide standard Unix quit notification
		// The "Quit: 3" message follows traditional Unix shell conventions
		// where 3 is the signal number for SIGQUIT
		write(STDOUT_FILENO, "Quit: 3\n", 8);
	}
	
	// SIGNAL CONTINUATION: After providing feedback, allow normal signal
	// processing to continue. The child process will terminate normally
	// and the parent will collect the appropriate exit status.
}

/*
** ============================================================================
** SIGNAL HANDLER CONFIGURATION AND SETUP
** ============================================================================
*/

/**
 * @brief Configures signal handlers for interactive shell operation
 * 
 * This function establishes the signal handling environment for the parent
 * shell process, implementing the specific signal behaviors required for
 * proper interactive shell operation. It uses the advanced sigaction()
 * interface for precise control over signal handling characteristics.
 * 
 * SIGNAL CONFIGURATION STRATEGY:
 * ==============================
 * The function configures two critical signals with distinct behaviors:
 * 
 * SIGINT (Interrupt Signal / Ctrl+C) Configuration:
 * - Handler: Custom signal_handler() function
 * - Flags: SA_RESTART for seamless system call integration
 * - Mask: Empty (no additional signals blocked during handling)
 * - Purpose: Interrupt current operation, display new prompt
 * 
 * SIGQUIT (Quit Signal / Ctrl+\) Configuration:
 * - Handler: SIG_IGN (completely ignored)
 * - Flags: None (default behavior for ignored signals)
 * - Mask: Empty (no additional signals blocked)
 * - Purpose: Prevent accidental shell termination
 * 
 * SA_RESTART FLAG SIGNIFICANCE:
 * ============================
 * The SA_RESTART flag is crucial for SIGINT handling because:
 * - Automatically restarts interrupted system calls
 * - Prevents readline() from being disrupted by signal delivery
 * - Ensures consistent behavior across different Unix systems
 * - Critical for GNU readline integration and proper terminal control
 * 
 * Without SA_RESTART, readline operations could fail unpredictably
 * when signals are delivered, leading to inconsistent user experience.
 * 
 * SIGACTION vs SIGNAL ADVANTAGES:
 * ===============================
 * Using sigaction() instead of signal() provides:
 * - Portable behavior across Unix systems
 * - Precise control over signal handling flags
 * - Ability to specify signal masks during handler execution
 * - More reliable and consistent signal semantics
 * - Better integration with modern Unix signal handling
 * 
 * SIGNAL MASK MANAGEMENT:
 * ======================
 * The function uses sigemptyset() to create empty signal masks:
 * - No additional signals are blocked during handler execution
 * - Allows maximum responsiveness to signal events
 * - Prevents potential deadlocks from overly restrictive masking
 * - Maintains simple and predictable signal handling behavior
 * 
 * INTERACTIVE SHELL REQUIREMENTS:
 * ==============================
 * Interactive shells have specific signal handling requirements:
 * - Must respond to Ctrl+C without terminating
 * - Should ignore Ctrl+\ to prevent accidental exit
 * - Must integrate cleanly with readline for prompt management
 * - Should provide immediate visual feedback for signal events
 * 
 * @note This function should only be called for the parent shell process
 * @note Signal handlers are inherited by child processes until overridden
 * @note Configuration affects all subsequent signal delivery
 */

void	setup_signals(void)
{
	struct sigaction	sa_int;    // Signal action structure for SIGINT
	struct sigaction	sa_quit;   // Signal action structure for SIGQUIT

	// SIGINT CONFIGURATION: Set up Ctrl+C handling for interactive mode
	
	// Initialize signal mask to empty set (no signals blocked during handler)
	sigemptyset(&sa_int.sa_mask);
	
	// Assign custom signal handler for interactive interrupt processing
	sa_int.sa_handler = signal_handler;
	
	// Set SA_RESTART flag for seamless readline integration
	// This is critical: without SA_RESTART, readline operations could be
	// interrupted unpredictably, leading to inconsistent terminal behavior
	sa_int.sa_flags = SA_RESTART;
	
	// Apply SIGINT configuration to the process
	sigaction(SIGINT, &sa_int, NULL);

	// SIGQUIT CONFIGURATION: Ignore Ctrl+\ in interactive mode
	
	// Initialize signal mask to empty set for quit signal
	sigemptyset(&sa_quit.sa_mask);
	
	// Set handler to SIG_IGN to completely ignore quit signals
	// This prevents accidental shell termination from Ctrl+\ presses
	sa_quit.sa_handler = SIG_IGN;
	
	// No special flags needed for ignored signals
	sa_quit.sa_flags = 0;
	
	// Apply SIGQUIT configuration to ignore quit attempts
	sigaction(SIGQUIT, &sa_quit, NULL);
}

/**
 * @brief Configures signal handlers for child process operation
 * 
 * This function establishes signal handling behavior for child processes
 * (commands executed by the shell), implementing Unix-standard signal
 * semantics that differ from the interactive shell. Child processes
 * should respond to signals in the traditional manner rather than the
 * specialized interactive behavior of the parent shell.
 * 
 * CHILD PROCESS SIGNAL REQUIREMENTS:
 * ==================================
 * Child processes have different signal handling needs than the parent:
 * - Should respond normally to termination signals
 * - Must provide appropriate feedback about signal-based termination
 * - Should NOT use SA_RESTART (different from parent)
 * - Must allow normal Unix signal propagation and termination
 * 
 * SIGNAL CONFIGURATION DIFFERENCES:
 * =================================
 * 
 * SIGINT (Ctrl+C) for Child Processes:
 * - Handler: Custom signal_handler_child() function
 * - Flags: No SA_RESTART (unlike parent)
 * - Purpose: Provide feedback, then allow normal termination
 * - Behavior: Set flag, output newline, allow signal to continue
 * 
 * SIGQUIT (Ctrl+\) for Child Processes:
 * - Handler: Custom signal_handler_child() function (not ignored like parent)
 * - Flags: No special flags needed
 * - Purpose: Provide quit message, then allow normal termination
 * - Behavior: Set flag, output quit message, allow signal to continue
 * 
 * WHY NO SA_RESTART FOR CHILDREN:
 * ===============================
 * Child processes intentionally omit SA_RESTART because:
 * - Child processes typically don't use readline
 * - Signals should interrupt system calls for proper termination
 * - Standard Unix behavior expects system call interruption
 * - Child processes should exit promptly when signaled
 * 
 * SIGNAL INHERITANCE AND RESTORATION:
 * ==================================
 * Child processes inherit signal handlers from the parent, so this function
 * "restores" more traditional Unix signal handling:
 * - Overrides parent's SIG_IGN for SIGQUIT
 * - Provides feedback while maintaining terminability
 * - Ensures child processes can be interrupted and terminated normally
 * - Maintains compatibility with standard Unix command behavior
 * 
 * PROCESS TERMINATION COORDINATION:
 * ================================
 * The signal handlers set global flags to coordinate with parent:
 * - Parent can detect which signal caused child termination
 * - Enables proper exit status calculation (128 + signal_number)
 * - Provides feedback without disrupting termination process
 * - Maintains standard Unix signal termination semantics
 * 
 * @note This function should only be called in child processes
 * @note Child signal handlers allow normal signal termination
 * @note No SA_RESTART flag is used for child processes
 */

void	setup_child_signals(void)
{
	struct sigaction	sa_int;    // Signal action structure for SIGINT
	struct sigaction	sa_quit;   // Signal action structure for SIGQUIT

	// SIGINT CONFIGURATION: Set up Ctrl+C handling for child processes
	
	// Initialize signal mask to empty set for child process
	sigemptyset(&sa_int.sa_mask);
	
	// Assign child-specific signal handler for interrupt processing
	sa_int.sa_handler = signal_handler_child;
	
	// NO SA_RESTART flag for child processes
	// This is intentional: child processes should allow system call interruption
	// for proper signal termination behavior
	sa_int.sa_flags = 0;
	
	// Apply SIGINT configuration for child process behavior
	sigaction(SIGINT, &sa_int, NULL);

	// SIGQUIT CONFIGURATION: Set up Ctrl+\ handling for child processes
	
	// Initialize signal mask to empty set for quit signal in child
	sigemptyset(&sa_quit.sa_mask);
	
	// Assign child-specific signal handler (not SIG_IGN like parent)
	// Child processes should respond to quit signals with appropriate feedback
	sa_quit.sa_handler = signal_handler_child;
	
	// No special flags needed for child quit signal handling
	sa_quit.sa_flags = 0;
	
	// Apply SIGQUIT configuration to enable quit signal processing
	sigaction(SIGQUIT, &sa_quit, NULL);
}
