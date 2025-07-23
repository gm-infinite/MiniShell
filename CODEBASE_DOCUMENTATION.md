# MiniShell Codebase Documentation

## Table of Contents
1. [Core Data Structures](#core-data-structures)
2. [Main Function Flow](#main-function-flow)
3. [Core Shell Functions](#core-shell-functions)
4. [Signal Handling](#signal-handling)
5. [Environment Management](#environment-management)
6. [Parsing and Tokenization](#parsing-and-tokenization)

---

## Core Data Structures

### `t_shell` Structure
The main shell structure that holds all the global state information:

```c
typedef struct s_shell
{
	int		past_exit_status;      // Exit status of the last executed command
	char	*current_input;        // The current command line input from user
	t_split	split_input;           // Tokenized version of current_input
	char	**envp;                // Environment variables array
	int		should_exit;           // Flag indicating if shell should exit
	int		exit_code;             // Exit code when shell terminates
	int		in_subshell;           // Flag indicating if currently in a subshell
	char	*prompt;               // The actual prompt string read from readline
	char	*terminal_prompt;      // The display prompt ("minishell > ")
} t_shell;
```

### `t_split` Structure
A custom structure for managing tokenized command strings:

```c
typedef struct s_split
{
	char	**start;    // Array of strings (tokens)
	int		size;       // Number of tokens in the array
} t_split;
```

---

## Main Function Flow

### `main(int argc, char **argv, char **envp)`
**Purpose**: Entry point of the minishell program.

**What it does**:
1. Ignores `argc` and `argv` parameters (not used)
2. Initializes the shell structure
3. Sets up the environment variables
4. Starts the main shell loop

**Functions used**:
- `shell_init()` - Initializes shell structure to default values
- `init_environment()` - Copies environment variables from system
- `start_shell()` - Begins the interactive shell loop

---

## Core Shell Functions

### `shell_init(t_shell *shell)`
**Purpose**: Initializes all fields of the shell structure to their default values.

**What it does**:
- Sets `current_input` to NULL
- Creates an empty `split_input` using `create_split()`
- Sets all status flags to 0
- Sets `envp` to NULL
- Sets `prompt` to NULL
- Sets `terminal_prompt` to "minishell > "

**Functions used**:
- `create_split(NULL, 0)` - Creates an empty t_split structure

### `start_shell(t_shell *shell)`
**Purpose**: Main interactive loop of the shell that handles user input and command execution.

**What it does**:
1. Sets up signal handlers
2. Enters infinite loop until `should_exit` is set
3. Reads user input (either from terminal via readline or from stdin)
4. Processes the input if it's not empty
5. Frees resources and exits when done

**Functions used**:
- `setup_signals()` - Configures signal handling for SIGINT and SIGQUIT
- `readline()` - Reads input from terminal (if interactive)
- `get_next_line()` - Reads input from stdin (if non-interactive)
- `add_history()` - Adds command to readline history
- `is_empty()` - Checks if input contains only whitespace
- `begin_command_parsing_and_execution()` - Processes and executes the command
- `safe_exit()` - Cleanly exits the shell
- `free_environment()` - Frees environment variables
- `rl_clear_history()` - Clears readline history

### `is_empty(char *str)`
**Purpose**: Checks if a string is empty or contains only whitespace characters.

**What it does**:
- Returns 1 if string is NULL
- Returns 1 if string contains only spaces, tabs, or control characters (9-13)
- Returns 0 if string contains any non-whitespace character

**Functions used**: None (only uses character checking)

### `safe_exit(t_shell *shell)`
**Purpose**: Cleanly terminates the shell by freeing all allocated memory.

**What it does**:
1. Frees `current_input` if not NULL
2. Frees `split_input` structure if allocated
3. Frees environment variables
4. Clears readline history
5. Exits with the stored exit status

**Functions used**:
- `free_split()` - Frees t_split structure and its contents
- `free_environment()` - Frees environment variables array
- `rl_clear_history()` - Clears readline history
- `exit()` - Terminates the program

---

## Signal Handling

### `setup_signals(void)`
**Purpose**: Configures signal handlers for interactive shell operation.

**What it does**:
1. Sets up SIGINT (Ctrl+C) handler to use `signal_handler()`
2. Sets up SIGQUIT (Ctrl+\) to be ignored (SIG_IGN)
3. Uses `sigaction()` for proper signal handling with SA_RESTART flag

**Functions used**:
- `sigemptyset()` - Initializes signal mask
- `sigaction()` - Sets signal handlers

### `signal_handler(int sig)`
**Purpose**: Handles SIGINT signals in the interactive shell.

**What it does**:
- Sets global variable `g_signal` to SIGINT
- Writes newline to stdout
- Uses readline functions to refresh the prompt display

**Functions used**:
- `write()` - Outputs newline
- `rl_on_new_line()` - Tells readline a new line started
- `rl_replace_line()` - Clears current line
- `rl_redisplay()` - Redisplays the prompt

---

## Environment Management

### `init_environment(t_shell *shell, char **envp)`
**Purpose**: Creates a copy of the system environment variables for the shell.

**What it does**:
1. Counts the number of environment variables
2. Allocates memory for the environment array
3. Creates duplicates of each environment string
4. Handles memory allocation failures gracefully

**Functions used**:
- `malloc()` - Allocates memory for environment array
- `ft_strdup()` - Creates copies of environment strings (libft)
- `free()` - Frees memory on allocation failure

### `free_environment(t_shell *shell)`
**Purpose**: Frees all memory allocated for environment variables.

**What it does**:
1. Iterates through all environment strings
2. Frees each individual string
3. Frees the environment array itself
4. Sets envp to NULL

**Functions used**:
- `free()` - Frees individual strings and array

### `get_env_value(char *var_name, t_shell *shell)`
**Purpose**: Retrieves the value of an environment variable by name.

**What it does**:
1. Searches through environment array for matching variable name
2. Finds the '=' character in each environment string
3. Compares variable names using exact length matching
4. Returns pointer to value part (after '=') if found

**Functions used**:
- `ft_strlen()` - Gets length of variable name (libft)
- `ft_strchr()` - Finds '=' character (libft)
- `ft_strncmp()` - Compares variable names (libft)

---

## Parsing and Tokenization

### `begin_command_parsing_and_execution(t_shell *shell)`
**Purpose**: Main command processing pipeline that handles parsing and execution.

**What it does**:
1. Validates quote matching in the input
2. Separates operators with spaces for proper tokenization
3. Expands wildcard patterns
4. Creates tokenized representation of the command
5. Executes the parsed command

**Functions used**:
- `check_quotes()` - Validates quote pairing
- `sep_opt_arg()` - Adds spaces around operators
- `wildcard_input_modify()` - Expands wildcard patterns
- `create_split_str()` - Tokenizes the command string
- `parser_and_or()` - Executes the parsed command
- `free_split()` - Frees tokenized structure

### `check_quotes(char *str)`
**Purpose**: Validates that quotes are properly paired in the input string.

**What it does**:
1. Tracks single quote state (ignores everything inside single quotes)
2. Tracks double quote state (ignores single quotes inside double quotes)
3. Returns 1 if all quotes are properly closed, 0 otherwise

**Functions used**: None (only uses character checking)

### `sep_opt_arg(t_shell *shell)`
**Purpose**: Adds spaces around shell operators to ensure proper tokenization.

**What it does**:
1. Calculates the size needed for the separated string
2. Creates a new string with spaces added around operators
3. Handles operators: "||", "&&", "<<", ">>", "|", ">", "<"
4. Ignores operators inside quotes

**Functions used**:
- `get_sep_size()` - Calculates required size for separated string
- `get_inquote()` - Checks if a position is inside quotes
- `set_sep()` - Performs the actual separation
- `ft_calloc()` - Allocates memory for new string (libft)
- `free()` - Frees old string

### `get_sep_size(char *un_sep)`
**Purpose**: Calculates how much extra space is needed to add spaces around operators.

**What it does**:
1. Iterates through the string looking for operators
2. Counts extra spaces needed before and after each operator
3. Uses `get_inquote()` to skip operators inside quotes
4. Returns total size needed including original string

**Functions used**:
- `get_inquote()` - Checks quote state at position
- `get_opt()` - Gets operator string by index
- `ft_strncmp()` - Compares operator strings (libft)

### `get_inquote(char *un_sep, int size)`
**Purpose**: Determines if a position in the string is inside quotes.

**What it does**:
1. Tracks quote state using XOR operations
2. Uses bit 1 for double quotes, bit 2 for single quotes
3. Returns the quote state at the given position
4. Properly handles nested quote scenarios

**Functions used**: None (only uses bitwise operations)

### `set_sep(char *sep, char *un_sep, int sep_size)`
**Purpose**: Actually performs the separation by adding spaces around operators.

**What it does**:
1. Copies characters from original string to new string
2. When an operator is found outside quotes, adds spaces before/after
3. Uses complex indexing to handle the space insertion
4. Maintains proper positioning throughout the process

**Functions used**:
- `get_inquote()` - Checks quote state
- `get_opt()` - Gets operator strings
- `ft_strncmp()` - Compares operators (libft)
- `ft_strlcat()` - Concatenates operator string (libft)

### `wildcard_input_modify(char *current_input)`
**Purpose**: Expands wildcard patterns (*) in the command input.

**What it does**:
1. Tokenizes the input into individual arguments
2. For each token containing '*' outside quotes, expands it
3. Replaces wildcard tokens with matching filenames
4. Reconstructs the command string with expanded arguments

**Functions used**:
- `create_split_str()` - Tokenizes the input
- `countchr_not_quote()` - Counts '*' characters outside quotes
- `wildcard_handle()` - Performs the actual wildcard expansion
- `revert_split_str()` - Reconstructs string from tokens
- `free_split()` - Frees tokenized structure

### `create_split_str(char *str)`
**Purpose**: Creates a t_split structure by tokenizing a command string.

**What it does**:
1. Uses `ft_split_quotes()` to split string respecting quotes
2. Counts the number of resulting tokens
3. Creates and returns a t_split structure

**Functions used**:
- `ft_split_quotes()` - Splits string while respecting quote boundaries
- `create_split()` - Creates t_split structure from array and size

### `ft_split_quotes(const char *s)`
**Purpose**: Splits a string into tokens while respecting quote boundaries.

**What it does**:
1. First pass: counts tokens by tracking quote states
2. Second pass: extracts each token preserving quoted sections
3. Handles both single and double quotes properly
4. Treats spaces and tabs as delimiters outside quotes

**Functions used**:
- `count_tokens()` - Counts number of tokens in string
- `extract_token()` - Extracts individual tokens
- `ft_substr()` - Creates substring for each token (libft)
- `malloc()` - Allocates memory for token array

---

## Function Dependencies Summary

**Main execution flow**:
```
main() → shell_init() → init_environment() → start_shell()
├── setup_signals()
└── Interactive loop:
    ├── readline() / get_next_line()
    ├── is_empty()
    └── begin_command_parsing_and_execution()
        ├── check_quotes()
        ├── sep_opt_arg()
        │   ├── get_sep_size()
        │   ├── get_inquote()
        │   └── set_sep()
        ├── wildcard_input_modify()
        ├── create_split_str()
        │   └── ft_split_quotes()
        └── parser_and_or() [to be explained later]
```

This documentation covers the fundamental functions of the minishell codebase from initialization through command parsing. Each function is explained with its purpose, implementation details, and dependencies on other functions.
