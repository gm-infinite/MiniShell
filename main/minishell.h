/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:53:30 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/14 18:38:26 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** MINISHELL.H - Comprehensive Shell Implementation Header
**
** PURPOSE:
** This header file serves as the central declaration hub for the entire minishell
** project. It defines all data structures, function prototypes, system includes,
** and global constants needed for a fully functional bash-like shell implementation.
** The design emphasizes modularity, type safety, and comprehensive functionality
** covering all aspects of shell operation from basic command execution to complex
** pipeline and redirection handling.
**
** ARCHITECTURE OVERVIEW:
** The header is organized into logical sections:
** 1. System Includes - All required POSIX and GNU library headers
** 2. Global Constants - PATH_MAX definition and signal externals
** 3. Core Data Structures - Shell state and command representation
** 4. Function Prototypes - Organized by functional area
** 5. Utility Macros - Helper definitions for common operations
**
** DESIGN PHILOSOPHY:
** - Comprehensive Coverage: Includes all necessary system headers
** - Type Safety: Explicit structure definitions with meaningful names
** - Modularity: Function prototypes grouped by logical functionality
** - Portability: Conditional compilation for cross-platform compatibility
** - Documentation: Every structure and major section thoroughly documented
*/

#ifndef MINISHELL_H
# define MINISHELL_H

/*
** ============================================================================
**                           SYSTEM INCLUDES
** ============================================================================
** Comprehensive collection of system headers required for full shell functionality.
** Each include serves specific purposes in the shell implementation:
*/

/* Standard I/O and Memory Management */
# include <stdio.h>        // printf, fprintf, perror - standard I/O operations
# include <stdlib.h>       // malloc, free, exit - memory and process management

/* GNU Readline Library - Interactive Command Line Interface */
# include <readline/readline.h>  // readline() - interactive input with editing
# include <readline/history.h>   // add_history() - command history management

/* POSIX Compliance and Feature Selection */
# define _POSIX_C_SOURCE 200809L  // Enable POSIX.1-2008 features explicitly

/* Signal and Process Management */
# include <signal.h>       // signal(), sigaction() - signal handling infrastructure
# include <sys/wait.h>     // wait(), waitpid() - child process synchronization

/* System Calls and File Operations */
# include <unistd.h>       // fork(), exec*(), pipe() - core system operations
# include <fcntl.h>        // open(), O_* flags - file control operations
# include <sys/stat.h>     // stat(), file permission constants
# include <dirent.h>       // opendir(), readdir() - directory operations

/* String Processing and Error Handling */
# include <string.h>       // strncmp(), strlen() - string manipulation
# include <errno.h>        // errno, error constants - error reporting

/* Terminal Control and I/O */
# include <termios.h>      // Terminal I/O control for signal handling
# include <sys/ioctl.h>    // Terminal size and control operations

/* System Limits and Constants */
# include <limits.h>       // System-defined limits (PATH_MAX, etc.)

/* Project-Specific Headers */
# include "../t_split_utils/t_split.h"  // Tokenization and parsing utilities

/*
** ============================================================================
**                           GLOBAL CONSTANTS
** ============================================================================
*/

/* PATH_MAX Definition - Maximum path length for cross-platform compatibility */
# ifndef PATH_MAX
#  define PATH_MAX 4096    // Standard maximum path length on most systems
# endif

/*
** GLOBAL SIGNAL VARIABLE
** Required by 42 school subject for signal handling between parent and child processes.
** This variable enables communication of signal events across the shell execution
** context while maintaining async-signal-safe operations.
**
** Design Notes:
** - sig_atomic_t ensures atomic reads/writes even during signal delivery
** - volatile prevents compiler optimization that might interfere with signal handling
** - extern declaration allows access from multiple source files
*/
extern volatile sig_atomic_t g_signal;

/*
** ============================================================================
**                        CORE DATA STRUCTURES
** ============================================================================
*/

/*
** SHELL STATE STRUCTURE - Central Shell Execution Context
**
** PURPOSE:
** The t_shell structure encapsulates all persistent state required for shell
** operation. It serves as the primary context object passed between functions,
** containing execution history, environment, and current command state.
**
** DESIGN RATIONALE:
** - Single Source of Truth: All shell state centralized in one structure
** - Context Passing: Eliminates need for global variables (except g_signal)
** - Memory Management: Clear ownership model for all dynamic allocations
** - Process Coordination: Supports both parent and child process contexts
**
** USAGE PATTERNS:
** - Initialized once at shell startup
** - Passed by reference to all major functions
** - Modified by built-ins to update persistent state
** - Copied/inherited by child processes as needed
*/
typedef struct s_shell
{
	/*
	** EXIT STATUS TRACKING
	** Stores the exit code from the last executed command/pipeline.
	** Used for $? variable expansion and && || conditional evaluation.
	** Updated after each command completion.
	*/
	int		past_exit_status;
	
	/*
	** CURRENT COMMAND PROCESSING STATE
	** Holds the raw input string and its tokenized representation.
	** These fields are updated for each new command line processed.
	*/
	char	*current_input;     // Raw command line string from readline()
	t_split	split_input;        // Tokenized representation of current command
	
	/*
	** PROCESS MANAGEMENT
	** Tracks child processes created during command execution.
	** Essential for proper process cleanup and signal handling.
	*/
	pid_t	*child_pids;        // Array of active child process IDs
	char	parent_or_child;    // Process type identifier ('P'=parent, 'C'=child)
	
	/*
	** ENVIRONMENT MANAGEMENT
	** Dynamic environment variable storage and manipulation.
	** Supports export/unset operations and variable expansion.
	*/
	char	**envp;             // Dynamic environment variable array
	
	/*
	** PIPELINE EXECUTION STATE
	** Tracks complex command execution context for proper resource management.
	*/
	/*
	** SHELL TERMINATION CONTROL
	** Flags used to control graceful shell shutdown and exit code propagation.
	** These fields enable clean termination from deeply nested execution contexts.
	*/
	int		should_exit;         // Flag indicating shell should terminate
	int		exit_code;           // Exit code to use when shell terminates
}			t_shell;

/*
** ============================================================================
**                        FUNCTION PROTOTYPES
** ============================================================================
** Comprehensive collection of function declarations organized by functional area.
** Each section represents a major subsystem of the shell implementation.
*/

/*
** ────────────────────────────────────────────────────────────────────────────
**                           CORE SHELL FUNCTIONS
** ────────────────────────────────────────────────────────────────────────────
** Primary shell control and utility functions for basic operation.
*/

/*
** SAFE_EXIT - Graceful Shell Termination Handler
** Performs comprehensive cleanup before shell exit including:
** - Memory deallocation for all dynamic structures
** - File descriptor cleanup and pipe closure
** - Process cleanup for any running children
** - Environment variable cleanup
** @shell: Shell state to clean up before exit
*/
void	safe_exit(t_shell *shell);

/*
** IS_EMPTY - Input Validation Utility
** Determines if a string contains only whitespace characters.
** Used to detect empty commands and avoid unnecessary processing.
** @str: String to check for emptiness
** @returns: 1 if string is empty/whitespace-only, 0 if contains content
*/
int		is_empty(char *str);

/*
** ────────────────────────────────────────────────────────────────────────────
**                           SIGNAL HANDLING SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Comprehensive signal management for interactive and batch operation modes.
*/

/*
** SETUP_SIGNALS - Interactive Shell Signal Configuration
** Configures signal handlers for the main shell process including:
** - SIGINT (Ctrl+C): Interrupt current operation, new prompt
** - SIGQUIT (Ctrl+\): Ignored in interactive mode
** - Proper signal masking for child process isolation
*/
void	setup_signals(void);

/*
** SIGNAL_HANDLER - Primary Signal Handler for Shell Process
** Handles signals delivered to the main shell process with appropriate
** response based on current shell state and signal type.
** @sig: Signal number received (SIGINT, SIGQUIT, etc.)
*/
void	signal_handler(int sig);

/*
** SIGNAL_HANDLER_CHILD - Child Process Signal Handler
** Specialized signal handler for child processes with different semantics
** than the interactive shell (allows termination, different output behavior).
** @sig: Signal number received
*/
void	signal_handler_child(int sig);

/*
** SETUP_CHILD_SIGNALS - Child Process Signal Configuration
** Configures appropriate signal handling for child processes including:
** - Restoration of default signal behavior for most signals
** - Proper signal masking for pipeline execution
** - Prevention of signal interference between parent and child
*/
void	setup_child_signals(void);

/*
** ────────────────────────────────────────────────────────────────────────────
**                        PARSING AND TOKENIZATION SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Advanced command line parsing with operator precedence and grouping support.
*/

/*
** PARSER_AND_OR - Logical Operator Parser with Parentheses Support
** Implements sophisticated parsing of && and || operators with proper
** precedence handling and parentheses grouping. Supports nested expressions
** and short-circuit evaluation semantics.
** @shell: Shell execution context
** @split: Tokenized command line to parse and execute
*/
void	parser_and_or(t_shell *shell, t_split split);

/*
** CHARACTER COUNTING UTILITIES - Quote and Character Analysis
** Specialized functions for analyzing character occurrences with quote awareness.
** Essential for proper parsing of complex command lines with nested quotes.
*/

/*
** COUNTCHR_STR - Basic Character Counting
** Counts occurrences of a specific character in a string without quote awareness.
** @str: String to analyze
** @c: Character to count
** @returns: Number of occurrences found
*/
int		countchr_str(char *str, char c);

/*
** COUNTCHR_QUOTE - Quote-Context Character Counting
** Counts character occurrences only within quoted sections of a string.
** Used for analyzing characters that should be treated literally.
** @str: String to analyze
** @c: Character to count within quotes
** @returns: Number of occurrences within quoted sections
*/
int		countchr_quote(char *str, char c);

/*
** COUNTCHR_NOT_QUOTE - Non-Quote Character Counting
** Counts character occurrences only outside quoted sections.
** Critical for identifying operators that should be parsed as commands.
** @str: String to analyze
** @c: Character to count outside quotes
** @returns: Number of occurrences outside quoted sections
*/
int		countchr_not_quote(char *str, char c);

/*
** PARENTHESES AND SYNTAX ANALYSIS
** Functions for validating and analyzing complex command structures.
*/

/*
** CHECK_SINGLE_PAR - Parentheses Grouping Validator
** Determines if a command is enclosed in a single pair of parentheses
** that can be safely removed without changing execution semantics.
** @split: Command tokens to analyze
** @returns: Non-zero if single parentheses grouping detected
*/
int		check_single_par(t_split split);

/*
** CHECK_SYMBOL - Symbol Occurrence Analyzer
** Searches for specific symbols/operators within a tokenized command with
** optional quote awareness and occurrence counting.
** @split: Tokenized command to search
** @find: Symbol/string to search for
** @flag: Search mode flags (quote awareness, etc.)
** @returns: Number of occurrences or boolean result based on flag
*/
int		check_symbol(t_split split, char *find, int flag);

/*
** SEP_OPT_ARG - Argument Separation and Processing
** Performs advanced argument separation and option processing for complex
** command lines with mixed operators and argument structures.
** @shell: Shell context for processing
*/
void	sep_opt_arg(t_shell *shell);

/*
** COUNT_STR_SPLIT - String Occurrence Counter in Token Array
** Counts occurrences of a specific string within a tokenized command array
** with support for various matching modes and quote awareness.
** @split: Token array to search
** @str: String to count occurrences of
** @flag: Search mode and options
** @returns: Number of matching occurrences found
*/
int		count_str_split(t_split split, const char *str, int flag);

/*
** ────────────────────────────────────────────────────────────────────────────
**                      ENVIRONMENT VARIABLE SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Dynamic environment management with variable expansion and modification support.
*/

/*
** EXPAND_VARIABLES - Environment Variable Expansion Engine
** Performs comprehensive variable expansion on input strings including:
** - $VAR and ${VAR} syntax support
** - Special variables ($?, $$, etc.)
** - Quote-aware expansion with proper escaping
** - Recursive expansion for nested variables
** @str: String containing variables to expand
** @shell: Shell context with environment
** @returns: Newly allocated string with variables expanded (NULL if no expansion)
*/
char	*expand_variables(char *str, t_shell *shell);

/*
** GET_ENV_VALUE - Environment Variable Retrieval
** Retrieves the value of a specific environment variable from the shell's
** dynamic environment array. Handles special variables and edge cases.
** @var_name: Name of variable to retrieve
** @shell: Shell context containing environment
** @returns: Value string (not allocated) or NULL if variable not found
*/
char	*get_env_value(char *var_name, t_shell *shell);

/*
** INIT_ENVIRONMENT - Environment Initialization
** Creates and initializes the shell's dynamic environment array from the
** initial environment provided at startup. Enables runtime modification.
** @shell: Shell context to initialize
** @envp: Initial environment array from main()
*/
void	init_environment(t_shell *shell, char **envp);

/*
** FREE_ENVIRONMENT - Environment Cleanup
** Properly deallocates all memory associated with the shell's dynamic
** environment including individual strings and the array itself.
** @shell: Shell context to clean up
*/
void	free_environment(t_shell *shell);

/*
** SET_ENV_VAR - Environment Variable Assignment
** Creates or modifies an environment variable in the shell's dynamic
** environment. Handles memory allocation and array resizing as needed.
** @var_name: Name of variable to set
** @value: Value to assign to variable
** @shell: Shell context to modify
** @returns: 0 on success, non-zero on failure
*/
int		set_env_var(char *var_name, char *value, t_shell *shell);

/*
** UNSET_ENV_VAR - Environment Variable Removal
** Removes an environment variable from the shell's dynamic environment
** including proper memory cleanup and array compaction.
** @var_name: Name of variable to remove
** @shell: Shell context to modify
** @returns: 0 on success, non-zero on failure
*/
int		unset_env_var(char *var_name, t_shell *shell);

/*
** ────────────────────────────────────────────────────────────────────────────
**                          BUILT-IN COMMANDS SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Implementation of shell built-in commands with proper argument handling.
*/

/*
** IS_BUILTIN - Built-in Command Detection
** Determines if a command name corresponds to a shell built-in function.
** Used for dispatch routing between built-in and external command execution.
** @cmd: Command name to check
** @returns: Non-zero if command is a built-in, 0 otherwise
*/
int		is_builtin(char *cmd);

/*
** EXECUTE_BUILTIN - Built-in Command Dispatcher
** Dispatches execution to the appropriate built-in command implementation
** based on command name. Handles argument validation and error reporting.
** @args: Null-terminated argument array (args[0] is command name)
** @shell: Shell context for built-in execution
** @returns: Exit status of built-in command
*/
int		execute_builtin(char **args, t_shell *shell);

/*
** INDIVIDUAL BUILT-IN COMMAND IMPLEMENTATIONS
** Each built-in command is implemented as a separate function with
** standardized argument handling and error reporting.
*/

/*
** BUILTIN_ECHO - Echo Command Implementation
** Prints arguments to stdout with optional newline suppression (-n flag).
** Supports escape sequence processing and proper argument spacing.
** @args: Arguments to echo
** @returns: Always 0 (echo doesn't fail)
*/
int		builtin_echo(char **args);

/*
** BUILTIN_CD - Change Directory Implementation
** Changes current working directory with support for:
** - Relative and absolute paths
** - Home directory expansion (~)
** - Error reporting for invalid paths
** @args: Directory path arguments
** @shell: Shell context (for environment access)
** @returns: 0 on success, non-zero on failure
*/
int		builtin_cd(char **args, t_shell *shell);

/*
** BUILTIN_PWD - Print Working Directory Implementation
** Displays current working directory using getcwd() system call.
** Handles potential buffer overflow and error conditions gracefully.
** @returns: 0 on success, non-zero on failure
*/
int		builtin_pwd(void);

/*
** BUILTIN_EXPORT - Environment Variable Export Implementation
** Creates or modifies environment variables with proper validation:
** - Variable name format checking
** - Value assignment parsing
** - Integration with shell's dynamic environment
** @args: Variable assignments (VAR=value format)
** @shell: Shell context to modify
** @returns: 0 on success, non-zero on validation failure
*/
int		builtin_export(char **args, t_shell *shell);

/*
** BUILTIN_UNSET - Environment Variable Removal Implementation
** Removes environment variables from the shell's dynamic environment.
** Validates variable names and handles non-existent variables gracefully.
** @args: Variable names to remove
** @shell: Shell context to modify
** @returns: 0 on success, non-zero on failure
*/
int		builtin_unset(char **args, t_shell *shell);

/*
** BUILTIN_ENV - Environment Display Implementation
** Displays all current environment variables in VAR=value format.
** Output is suitable for shell evaluation or export to other processes.
** @shell: Shell context containing environment
** @returns: Always 0
*/
int		builtin_env(t_shell *shell);

/*
** BUILTIN_EXIT - Shell Termination Implementation
** Terminates the shell with optional exit code specification:
** - Validates numeric exit codes
** - Performs graceful cleanup
** - Sets termination flags in shell context
** @args: Optional exit code argument
** @shell: Shell context to terminate
** @returns: Exit code (or never returns)
*/
int		builtin_exit(char **args, t_shell *shell);

/*
** ────────────────────────────────────────────────────────────────────────────
**                        COMMAND EXECUTION SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Comprehensive command execution with pipeline and redirection support.
*/

/*
** EXECUTE_COMMAND - Primary Command Execution Dispatcher
** Main entry point for command execution. Analyzes command structure and
** routes to appropriate execution strategy (simple, pipeline, complex).
** @split: Tokenized command to execute
** @shell: Shell execution context
** @returns: Exit status of command execution
*/
int		execute_command(t_split split, t_shell *shell);

/*
** EXECUTE_PIPELINE - Pipeline Execution Engine
** Orchestrates execution of multi-command pipelines with proper:
** - Process creation and coordination
** - Inter-process communication via pipes
** - Resource cleanup and synchronization
** @split: Pipeline command tokens
** @shell: Shell execution context
** @returns: Exit status of pipeline (from last command)
*/
int		execute_pipeline(t_split split, t_shell *shell);

/*
** EXECUTE_PIPELINE_WITH_REDIRECTIONS - Enhanced Pipeline Executor
** Extended pipeline execution with integrated redirection support.
** Combines pipeline and redirection processing for complex commands.
** @split: Command tokens with pipeline and redirection operators
** @shell: Shell execution context
** @returns: Exit status of complex command execution
*/
int		execute_pipeline_with_redirections(t_split split, t_shell *shell);

/*
** EXECUTE_SINGLE_COMMAND - Simple Command Executor
** Optimized execution path for single commands without pipelines.
** Handles built-in detection, PATH resolution, and basic execution.
** @args: Command arguments array
** @shell: Shell execution context
** @returns: Exit status of command
*/
int		execute_single_command(char **args, t_shell *shell);

/*
** PIPELINE UTILITY FUNCTIONS
** Supporting functions for pipeline analysis and management.
*/

/*
** COUNT_PIPES - Pipeline Analysis Function
** Counts pipe operators in a command to determine pipeline complexity
** and resource requirements for execution setup.
** @split: Command tokens to analyze
** @returns: Number of pipe operators found
*/
int		count_pipes(t_split split);

/*
** SPLIT_BY_PIPES - Pipeline Segmentation Function
** Breaks a pipeline command into individual command segments for
** separate process execution. Each segment becomes one pipeline stage.
** @split: Complete pipeline command
** @cmd_count: Output parameter for number of command segments
** @returns: Array of command segments (NULL on failure)
*/
t_split	*split_by_pipes(t_split split, int *cmd_count);

/*
** EXECUTE_PIPE_COMMAND - Pipeline Component Executor
** Executes a single command within a pipeline context with proper
** I/O redirection and resource management.
** @cmd: Individual command segment
** @input_fd: Input file descriptor (stdin or pipe read end)
** @output_fd: Output file descriptor (stdout or pipe write end)
** @shell: Shell execution context
** @all_pipes: Array of all pipeline pipes for cleanup
** @pipe_count: Total number of pipes in pipeline
** @returns: Process ID of created child or -1 on failure
*/
int		execute_pipe_command(t_split cmd, int input_fd, int output_fd, t_shell *shell, int **all_pipes, int pipe_count);

/*
** EXECUTE_PIPE_CHILD - Pipeline Child Process Handler
** Runs in child process context to execute one pipeline component.
** Handles I/O redirection, argument processing, and command dispatch.
** @cmd: Command to execute in child
** @cmd_index: Position in pipeline (for I/O setup)
** @pipes: Array of pipeline pipes
** @cmd_count: Total commands in pipeline
** @shell: Shell execution context
*/
void	execute_pipe_child(t_split cmd, int cmd_index, int **pipes, int cmd_count, t_shell *shell);

/*
** EXECUTE_PIPE_CHILD_WITH_REDIRECTIONS - Enhanced Child Executor
** Extended child process execution with integrated redirection processing.
** Combines pipeline I/O with file redirection for complex scenarios.
** @cmd: Command with potential redirections
** @cmd_index: Position in pipeline
** @pipes: Pipeline pipe array
** @cmd_count: Total pipeline commands
** @shell: Shell execution context
*/
void	execute_pipe_child_with_redirections(t_split cmd, int cmd_index, int **pipes, int cmd_count, t_shell *shell);

/*
** ────────────────────────────────────────────────────────────────────────────
**                          I/O REDIRECTION SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Comprehensive input/output redirection with file and pipe support.
*/

/*
** IS_REDIRECTION - Redirection Operator Detection
** Identifies redirection operators in command tokens including:
** - Input redirection (<)
** - Output redirection (>, >>)
** - Error redirection (2>)
** - Here document (<<)
** @token: Token to check for redirection operator
** @returns: Redirection type code (0 if not a redirection)
*/
int		is_redirection(char *token);

/*
** HANDLE_HERE_DOC - Here Document Processor
** Implements here document functionality (<<) by reading multiple lines
** until delimiter is encountered. Creates pipe for data transfer.
** @delimiter: End marker for here document input
** @pipe_fd: Output parameter for pipe file descriptors
** @returns: 0 on success, non-zero on failure
*/
int		handle_here_doc(char *delimiter, int *pipe_fd);

/*
** PARSE_REDIRECTIONS - Redirection Parser and Processor
** Parses redirection operators from command tokens and sets up appropriate
** file descriptors. Removes redirection tokens from argument array.
** @split: Command tokens including redirections
** @input_fd: Output parameter for input file descriptor
** @output_fd: Output parameter for output file descriptor  
** @stderr_fd: Output parameter for error file descriptor
** @returns: Cleaned argument array (NULL on failure)
*/
char	**parse_redirections(t_split split, int *input_fd, int *output_fd, int *stderr_fd);

/*
** EXECUTE_WITH_REDIRECTIONS - Redirection-Aware Command Executor
** Executes commands with comprehensive redirection support including:
** - File descriptor setup and cleanup
** - Built-in command redirection handling
** - External command execution with redirected I/O
** @split: Command with redirection operators
** @shell: Shell execution context
** @returns: Exit status of redirected command
*/
int		execute_with_redirections(t_split split, t_shell *shell);

/*
** ────────────────────────────────────────────────────────────────────────────
**                            QUOTE PROCESSING SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Advanced quote handling with variable expansion and escape processing.
*/

/*
** CHECK_QUOTES - Quote Validation Function
** Validates quote pairing and nesting in input strings to detect
** syntax errors before processing. Handles both single and double quotes.
** @str: String to validate for quote correctness
** @returns: 0 if quotes are properly paired, non-zero on error
*/
int		check_quotes(char *str);

/*
** PROCESS_QUOTES - Quote Processing and Removal
** Processes quoted strings by removing quote characters and handling
** escape sequences. Integrates with variable expansion for proper semantics.
** @str: String with quotes to process
** @shell: Shell context for variable expansion
** @returns: Processed string with quotes removed (newly allocated)
*/
char	*process_quotes(char *str, t_shell *shell);

/*
** PROCESS_ARGS_QUOTES - Argument Array Quote Processor
** Applies quote processing to an entire argument array in-place.
** Handles mixed quoted and unquoted arguments properly.
** @args: Argument array to process
** @shell: Shell context for processing
*/
void	process_args_quotes(char **args, t_shell *shell);

/*
** EXPAND_VARIABLES_QUOTED - Quote-Aware Variable Expansion
** Performs variable expansion with proper quote context handling:
** - No expansion within single quotes
** - Full expansion within double quotes
** - Escape sequence handling for special characters
** @str: String to expand with quote awareness
** @shell: Shell context for expansion
** @returns: Expanded string (newly allocated) or NULL if no expansion
*/
char	*expand_variables_quoted(char *str, t_shell *shell);

/*
** ────────────────────────────────────────────────────────────────────────────
**                              UTILITY FUNCTIONS
** ────────────────────────────────────────────────────────────────────────────
** General-purpose utility functions for common operations.
*/

/*
** SPLIT_TO_ARGS - Token Array Converter
** Converts a t_split structure to a standard null-terminated string array
** suitable for use with exec functions and built-in commands.
** @split: Token structure to convert
** @returns: Null-terminated string array (newly allocated)
*/
char	**split_to_args(t_split split);

/*
** REVERT_SPLIT_STR - Split Structure to String Converter
** Converts a t_split structure back to a single string with tokens
** separated by spaces, useful for reconstructing processed input.
** @split: Token structure to convert
** @returns: String with space-separated tokens (newly allocated)
*/
char	*revert_split_str(t_split split);

/*
** FREE_ARGS - Argument Array Cleanup
** Properly deallocates a null-terminated string array including all
** individual strings and the array itself.
** @args: String array to deallocate
*/
void	free_args(char **args);

/*
** FIND_EXECUTABLE - PATH Resolution Function
** Locates an executable command in the system PATH or validates an
** absolute/relative path. Handles PATH searching and file permissions.
** @cmd: Command name or path to resolve
** @shell: Shell context (for PATH environment variable)
** @returns: Full path to executable (newly allocated) or NULL if not found
*/
char	*find_executable(char *cmd, t_shell *shell);

/*
** COMPACT_ARGS - Argument Array Compaction
** Removes empty strings from argument array and shifts remaining elements.
** Used after variable expansion to handle cases like "$EMPTY cmd args".
** @args: NULL-terminated string array to compact in-place
*/
void	compact_args(char **args);

/*
** ────────────────────────────────────────────────────────────────────────────
**                           WILDCARD EXPANSION SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** File pattern matching and globbing with * wildcard support.
*/

/*
** WILDCARD_INPUT_MODIFY - Wildcard Expansion Engine
** Processes input string to expand wildcard patterns (*) to matching filenames
** in the current directory. Handles quote-aware expansion to prevent expansion
** of wildcards within quotes.
** @current_input: Input string potentially containing wildcard patterns
** @shell: Shell context for variable expansion in quoted sections
** @returns: Modified string with wildcards expanded to matching files (or original if no matches)
*/
char	*wildcard_input_modify(char *current_input, t_shell *shell);

#endif  /* MINISHELL_H */