# MiniShell execute_pipe_redir Function Documentation

This document provides detailed explanation of the `execute_pipe_redir` function in the minishell project, which handles commands with both pipe operators and redirection operators.

## Function Overview

### `execute_pipe_redir(t_split split, t_shell *shell)`
**Location:** `execute/pipeline_utils.c`

**Purpose:** Executes commands with both pipe operators and redirection operators, handling the complex interaction between these two systems.

**Function Signature:**
```c
int execute_pipe_redir(t_split split, t_shell *shell)
```

**Parameters:**
- **split:** Tokenized command containing both pipes and redirections
- **shell:** Shell state structure for environment and status management

**Return Value:** Exit status of the pipeline execution

## Algorithm and Implementation

### Core Algorithm
```c
int execute_pipe_redir(t_split split, t_shell *shell)
{
    t_split *commands;
    int     cmd_count;
    int     exit_status;

    commands = split_by_pipes(split, &cmd_count);          // Split into command segments
    if (!commands)
        return (1);                                        // Memory allocation failed
    
    if (cmd_count == 1)                                    // Only one command
    {
        exit_status = execute_with_redirections(commands[0], shell);
        free(commands);
        return (exit_status);
    }
    
    return (setup_and_execute_pipeline(commands, cmd_count, shell));
}
```

### Execution Flow
1. **Command Splitting:** Separates the input into individual command segments at pipe operators
2. **Single Command Check:** If no pipes exist, delegates to redirection-only execution
3. **Pipeline Setup:** For multiple commands, sets up pipeline with both pipes and redirections
4. **Resource Management:** Ensures proper cleanup of all allocated resources

## Core Pipeline Functions

### `split_by_pipes(t_split split, int *cmd_count)`
**Location:** `execute/pipes/split_by_pipes.c` (lines 66-79)

```c
t_split *split_by_pipes(t_split split, int *cmd_count)
{
    t_split *commands;
    int     pipe_count;

    pipe_count = count_pipes(split);                       // Count pipe operators
    *cmd_count = pipe_count + 1;                          // Commands = pipes + 1
    if (pipe_count == 0)
        return (allocate_single_command(split));           // No pipes, return single command
    
    commands = malloc(sizeof(t_split) * (*cmd_count));     // Allocate command array
    if (!commands)
        return (NULL);
    
    split_commands_by_pipes(split, commands);              // Split at pipe boundaries
    return (commands);
}
```

**Purpose:** Divides the input command into separate command segments split by pipe operators.

**Algorithm:**
1. **Pipe Counting:** Determines number of pipe operators to calculate command count
2. **Memory Allocation:** Allocates array for individual command segments
3. **Boundary Detection:** Identifies pipe positions and creates command boundaries
4. **Segment Creation:** Creates t_split structures for each command segment

### `count_pipes(t_split split)`
**Location:** `execute/pipes/split_by_pipes.c` (lines 15-26)

```c
int count_pipes(t_split split)
{
    int i;
    int pipe_count;

    pipe_count = 0;
    i = 0;
    while (i < split.size)
    {
        if (ft_strncmp(split.start[i], "|", 2) == 0)       // Found pipe operator
            pipe_count++;
        i++;
    }
    return (pipe_count);
}
```

**Purpose:** Counts the number of pipe operators in the command for resource allocation planning.

**Usage:** Determines command count (pipe_count + 1) and pipeline complexity.

### `allocate_single_command(t_split split)` (static)
**Location:** `execute/pipes/split_by_pipes.c` (lines 28-36)

```c
static t_split *allocate_single_command(t_split split)
{
    t_split *commands;

    commands = malloc(sizeof(t_split));                    // Allocate single command
    if (!commands)
        return (NULL);
    commands[0] = split;                                   // Copy entire split
    return (commands);
}
```

**Purpose:** Creates a single-element command array for commands without pipes.

**Optimization:** Avoids unnecessary splitting when no pipes are present.

### `split_commands_by_pipes(t_split split, t_split *commands)` (static)
**Location:** `execute/pipes/split_by_pipes.c` (lines 38-63)

```c
static void split_commands_by_pipes(t_split split, t_split *commands)
{
    int i;
    int cmd_idx;
    int start;

    cmd_idx = 0;
    start = 0;
    i = 0;
    while (i <= split.size)
    {
        if (i == split.size || (ft_strncmp(split.start[i], "|", 2) == 0))
        {
            commands[cmd_idx].start = &split.start[start]; // Point to segment start
            commands[cmd_idx].size = i - start;            // Calculate segment size
            cmd_idx++;
            start = i + 1;                                 // Next segment starts after pipe
        }
        i++;
    }
}
```

**Purpose:** Internal function that performs the actual splitting at pipe boundaries.

**Memory Efficiency:** Creates segments by pointing to existing token array rather than copying.

### Redirection Integration

For commands that only have redirections without pipes, this function delegates to:

**`execute_with_redirections(t_split split, t_shell *shell)`**

**Note:** This function is documented in detail in **[EXECUTE_WITH_REDIRECTIONS_DOCUMENTATION.md](EXECUTE_WITH_REDIRECTIONS_DOCUMENTATION.md)**.

This integration ensures proper handling of:
- Input redirections (`<`)
- Output redirections (`>`, `>>`)
- Here documents (`<<`)
- Error redirection
- File descriptor management

## Pipeline Setup and Execution

### `setup_and_execute_pipeline(t_split *commands, int cmd_count, t_shell *shell)` (static)
**Location:** `execute/execute_redirection_utils.c` (lines 15-30)

```c
static int setup_and_execute_pipeline(t_split *commands, int cmd_count, t_shell *shell)
{
    int         **pipes;
    pid_t       *pids;
    t_pipe_ctx  pipeline_ctx;

    if (!setup_pipe(&commands, &pipes, &pids, cmd_count))  // Initialize resources
        return (1);
    
    pipeline_ctx.commands = commands;                      // Setup context structure
    pipeline_ctx.pipes = pipes;
    pipeline_ctx.pids = pids;
    pipeline_ctx.cmd_count = cmd_count;
    pipeline_ctx.shell = shell;
    
    cmd_count = exec_pipe_child(&pipeline_ctx);            // Execute pipeline
    setup_signals();                                       // Restore signal handling
    clean_pipe(commands, pipes, pids, pipeline_ctx.cmd_count); // Cleanup resources
    
    return (cmd_count);
}
```

**Purpose:** Coordinates the complete pipeline setup and execution process.

**Integration:** Bridges resource setup with execution and cleanup phases.

### `setup_pipe(t_split **commands, int ***pipes, pid_t **pids, int cmd_count)`
**Location:** `execute/pipeline_utils.c` (lines 54-75)

```c
int setup_pipe(t_split **commands, int ***pipes, pid_t **pids, int cmd_count)
{
    if (!create_pipes_array(pipes, cmd_count))             // Create pipe array
    {
        free(*commands);
        *commands = NULL;
        return (0);
    }
    
    *pids = malloc(sizeof(pid_t) * cmd_count);             // Allocate PID array
    if (!*pids)
    {
        cleanup_pipes_safe(*pipes, cmd_count);             // Cleanup on failure
        *pipes = NULL;
        free(*commands);
        *commands = NULL;
        return (0);
    }
    return (1);
}
```

**Purpose:** Allocates and initializes all resources needed for pipeline execution.

**Error Handling:** Provides comprehensive cleanup on any allocation failure.

### `create_pipes_array(int ***pipes, int cmd_count)`
**Location:** `execute/pipe_management.c` (lines 30-53)

```c
int create_pipes_array(int ***pipes, int cmd_count)
{
    int i;

    *pipes = malloc(sizeof(int *) * (cmd_count - 1));      // Allocate pipe array
    if (!*pipes)
        return (0);
    
    i = 0;
    while (i < cmd_count - 1)
    {
        (*pipes)[i] = malloc(sizeof(int) * 2);             // Allocate fd pair
        if (!(*pipes)[i])
        {
            while (--i >= 0)                               // Cleanup on failure
                free((*pipes)[i]);
            free(*pipes);
            *pipes = NULL;
            return (0);
        }
        if (pipe((*pipes)[i]) == -1)                       // Create actual pipe
            return (close_fds_and_pipes(pipes, &i));
        i++;
    }
    return (1);
}
```

**Purpose:** Creates the array of pipe file descriptor pairs needed for command communication.

**Pipe Count Logic:** Creates (cmd_count - 1) pipes for cmd_count commands.

### `close_fds_and_pipes(int ***pipes, int *i)` (static)
**Location:** `execute/pipe_management.c` (lines 15-25)

```c
static int close_fds_and_pipes(int ***pipes, int *i)
{
    perror("pipe");                                        // Report pipe creation error
    free((*pipes)[*i]);                                    // Free failed pipe allocation
    while (--*i >= 0)
    {
        close((*pipes)[*i][0]);                            // Close read end
        close((*pipes)[*i][1]);                            // Close write end
        free((*pipes)[*i]);                                // Free pipe structure
    }
    free(*pipes);
    *pipes = NULL;
    return (0);
}
```

**Purpose:** Error recovery function that cleans up partially created pipe arrays.

**Safety:** Ensures no file descriptor leaks when pipe creation fails.

### `cleanup_pipes_safe(int **pipes, int cmd_count)` (static)
**Location:** `execute/pipeline_utils.c` (lines 39-53)

```c
static void cleanup_pipes_safe(int **pipes, int cmd_count)
{
    int i;

    if (!pipes)
        return;
    
    i = 0;
    while (i < cmd_count - 1 && pipes[i])
    {
        if (pipes[i][0] >= 0)                              // Valid read fd
            close(pipes[i][0]);
        if (pipes[i][1] >= 0)                              // Valid write fd
            close(pipes[i][1]);
        free(pipes[i]);                                    // Free pipe structure
        pipes[i] = NULL;
        i++;
    }
    free(pipes);
}
```

**Purpose:** Safe cleanup function that handles partially initialized pipe arrays.

**Safety Features:**
- **Null Checking:** Handles NULL pipe arrays gracefully
- **FD Validation:** Only closes valid file descriptors
- **Double-Free Prevention:** Sets freed pointers to NULL

### `exec_pipe_child(t_pipe_ctx *pipeline_ctx)`
**Location:** `execute/pipeline_utils.c` (lines 77-87)

```c
int exec_pipe_child(t_pipe_ctx *pipeline_ctx)
{
    int i;

    if (heredoc_pipe(pipeline_ctx) != 0)                   // Handle here documents
        return (1);
    
    i = -1;
    while (++i < pipeline_ctx->cmd_count)
    {
        if (fork_pipe_child(pipeline_ctx, i) != 0)         // Fork each command
            return (1);
    }
    
    close_all_pipes(pipeline_ctx->pipes, pipeline_ctx->cmd_count); // Close parent pipes
    return (wait_for_others(pipeline_ctx->pids, pipeline_ctx->cmd_count)); // Wait for children
}
```

**Purpose:** Executes all commands in the pipeline by forking child processes.

**Process Flow:**
1. **Here Document Processing:** Handles `<<` redirections first
2. **Process Creation:** Forks child process for each command
3. **Parent Cleanup:** Closes all pipe file descriptors in parent
4. **Wait Coordination:** Waits for all child processes to complete

## Child Process Management

### `heredoc_pipe(t_pipe_ctx *pipeline_ctx)`
**Location:** `execute/pipeline_child_utils.c` (lines 46-69)

```c
int heredoc_pipe(t_pipe_ctx *pipeline_ctx)
{
    int     i;
    char    **args;

    i = 0;
    while (i < pipeline_ctx->cmd_count)
    {
        args = split_to_args(pipeline_ctx->commands[i]);    // Convert to argv format
        if (!args)
        {
            i++;
            continue;
        }
        if (process_command_redirections(args, pipeline_ctx, i) != 0)
        {
            free_args(args);
            return (1);
        }
        free_args(args);
        i++;
    }
    return (0);
}
```

**Purpose:** Processes here document redirections for all commands in the pipeline before execution.

**Process Flow:**
1. **Command Iteration:** Processes each command in the pipeline
2. **Argument Conversion:** Converts t_split to argv format for redirection processing
3. **Redirection Processing:** Handles all redirections, especially here documents
4. **Resource Cleanup:** Frees argument arrays after processing

### `split_to_args(t_split split)`
**Location:** `execute/exec_utils.c` (lines 15-25)

```c
char **split_to_args(t_split split)
{
    char **args;

    args = allocate_args_array(split.size);                // Allocate argv array
    if (!args)
        return (NULL);
    if (copy_split_strings(args, split) == -1)             // Copy strings with error handling
        return (NULL);
    return (args);
}
```

**Purpose:** Converts t_split structure to standard argv format for command execution.

**Memory Management:** Allocates new memory and copies strings to create independent argv array.

### `allocate_args_array(int size)`
**Location:** `execute/exec_args_utils.c` (lines 15-21)

```c
char **allocate_args_array(int size)
{
    char **args;

    args = malloc(sizeof(char *) * (size + 1));            // +1 for NULL terminator
    return (args);
}
```

**Purpose:** Allocates memory for argv array with proper NULL termination.

**Array Size:** Allocates space for all arguments plus NULL terminator.

### `copy_split_strings(char **args, t_split split)`
**Location:** `execute/exec_args_utils.c` (lines 30-51)

```c
int copy_split_strings(char **args, t_split split)
{
    int i;

    i = 0;
    while (i < split.size)
    {
        if (split.start[i])
        {
            args[i] = ft_strdup(split.start[i]);           // Duplicate each string
            if (!args[i])
            {
                cleanup_args_on_error(args, i);           // Cleanup on failure
                return (-1);
            }
        }
        else
            args[i] = NULL;
        i++;
    }
    args[i] = NULL;                                        // NULL-terminate array
    return (0);
}
```

**Purpose:** Copies strings from t_split to argv array with error handling.

**Error Recovery:** Calls cleanup function if any string duplication fails.

### `cleanup_args_on_error(char **args, int up_to_index)` (static)
**Location:** `execute/exec_args_utils.c` (lines 23-28)

```c
static void cleanup_args_on_error(char **args, int up_to_index)
{
    while (--up_to_index >= 0)
        free(args[up_to_index]);                           // Free successfully allocated strings
    free(args);                                            // Free the array itself
}
```

**Purpose:** Cleans up partially allocated argv array when string duplication fails.

**Partial Cleanup:** Only frees strings that were successfully allocated.

### `process_command_redirections(char **args, t_pipe_ctx *pipeline_ctx, int i)` (static)
**Location:** `execute/pipeline_child_utils.c` (lines 16-42)

```c
static int process_command_redirections(char **args, t_pipe_ctx *pipeline_ctx, int i)
{
    int j;

    j = 0;
    while (args[j])
    {
        if (is_redirection(args[j]) == 1)                  // Found here document
        {
            if (!args[j + 1])                              // No delimiter provided
            {
                write(STDERR_FILENO, "syntax error: missing filename\n", 32);
                return (1);
            }
            if (heredoc_redir(args, j, pipeline_ctx, i) != 0)
                return (1);
            j += 2;                                        // Skip operator and delimiter
        }
        else
        {
            if (is_redirection(args[j]))
                j += 2;                                    // Skip other redirections
            else
                j++;
        }
    }
    return (0);
}
```

**Purpose:** Processes redirection operators in command arguments, focusing on here documents.

**Redirection Types:** Specifically handles here document (`<<`) redirections in pipeline context.

## Here Document Processing

### `heredoc_redir(char **args, int j, t_pipe_ctx *pipeline_ctx, int i)`
**Location:** `execute/pipeline_process_utils.c` (lines 106-130)

```c
int heredoc_redir(char **args, int j, t_pipe_ctx *pipeline_ctx, int i)
{
    char            *processed_delimiter;
    t_heredoc_sub   heresub;
    char            *temp_filename;

    heresub.args = args;
    heresub.temp_fd = setup_heredoc_file(&processed_delimiter, &temp_filename,
            i, args[j + 1]);                               // Setup temp file
    if (heresub.temp_fd == -1)
        return (1);
    
    heresub.should_expand = !delimiter_was_quoted(args[j + 1]); // Check expansion
    if (heredoc_subprocess(heresub, processed_delimiter, pipeline_ctx,
            temp_filename) != 0)
    {
        close(heresub.temp_fd);
        return (1);
    }
    
    close(heresub.temp_fd);
    update_command_filename(pipeline_ctx, i, j, temp_filename); // Replace with temp file
    free(processed_delimiter);
    return (0);
}
```

**Purpose:** Handles complete here document processing including file creation and content writing.

**Process Steps:**
1. **File Setup:** Creates temporary file for here document content
2. **Expansion Check:** Determines if variable expansion should occur
3. **Content Processing:** Writes here document content via subprocess
4. **Command Update:** Replaces delimiter with temporary filename

### `t_heredoc_sub` Structure
**Location:** `main/minishell.h` (lines 54-59)

```c
typedef struct s_heredoc_sub
{
    int     temp_fd;        // File descriptor for temporary file
    int     should_expand;  // Whether to expand variables in content
    char    **args;         // Command arguments array
}   t_heredoc_sub;
```

**Purpose:** Contains context data for here document processing.

**Fields:**
- **temp_fd:** File descriptor for writing here document content
- **should_expand:** Flag indicating if variable expansion should occur
- **args:** Original command arguments for context

### `setup_heredoc_file(char **processed_delimiter, char **temp_filename, int cmd_index, char *delimiter)` (static)
**Location:** `execute/pipeline_process_utils.c` (lines 67-88)

```c
static int setup_heredoc_file(char **processed_delimiter, char **temp_filename,
        int cmd_index, char *delimiter)
{
    int temp_fd;

    *processed_delimiter = process_heredoc_delimiter(delimiter);   // Remove quotes
    if (!*processed_delimiter)
        return (-1);
    
    *temp_filename = heredoc_filename(cmd_index);                  // Generate unique filename
    if (!*temp_filename)
    {
        free(*processed_delimiter);
        return (-1);
    }
    
    temp_fd = open(*temp_filename, O_WRONLY | O_CREAT | O_TRUNC, 0600); // Create temp file
    if (temp_fd == -1)
    {
        free(*processed_delimiter);
        free(*temp_filename);
        return (-1);
    }
    return (temp_fd);
}
```

**Purpose:** Sets up temporary file and processes delimiter for here document.

**Resource Management:** Comprehensive cleanup on any failure during setup.

### `process_heredoc_delimiter(char *filename)`
**Location:** `execute/redirections/redir_process_utils.c` (lines 46-52)

```c
char *process_heredoc_delimiter(char *filename)
{
    char *result;

    result = remove_quotes_for_redirection(filename);      // Remove quotes from delimiter
    return (result);
}
```

**Purpose:** Processes here document delimiter by removing quotes.

### `remove_quotes_for_redirection(char *str)`
**Location:** `execute/redirections/redir_syntax.c` (lines 29-end)

```c
char *remove_quotes_for_redirection(char *str)
{
    // Implementation removes surrounding quotes from redirection targets
    // Handles both single and double quotes
    // Returns newly allocated string without quotes
}
```

**Purpose:** Removes quotes from redirection targets and delimiters.

### `heredoc_filename(int cmd_index)`
**Location:** `execute/heredoc_utils.c` (lines 24-42)

```c
char *heredoc_filename(int cmd_index)
{
    char            *result;
    char            *temp_str;
    unsigned long   addr;
    int             local_var;

    addr = (unsigned long)&local_var;                      // Get stack address for uniqueness
    result = ft_strdup("/tmp/.minishell_heredoc_");
    temp_str = ft_itoa(addr % 1000000);                    // Add address component
    result = ft_realloc_join(result, temp_str);
    free(temp_str);
    result = ft_realloc_join(result, "_");
    temp_str = ft_itoa(cmd_index);                         // Add command index
    result = ft_realloc_join(result, temp_str);
    free(temp_str);
    return (result);
}
```

**Purpose:** Generates unique temporary filename for here document content.

**Uniqueness Strategy:** Combines stack address and command index for filename uniqueness.

### `ft_realloc_join(char *s1, char *s2)` (static)
**Location:** `execute/heredoc_utils.c` (lines 15-21)

```c
static char *ft_realloc_join(char *s1, char *s2)
{
    char *result;

    result = ft_strjoin(s1, s2);                           // Join strings
    free(s1);                                              // Free original
    return (result);
}
```

**Purpose:** Joins two strings and frees the first one, useful for string building.

**Memory Management:** Prevents memory leaks during string concatenation.

### `delimiter_was_quoted(char *filename)`
**Location:** `execute/redirections/redir_process_utils.c` (lines 30-end)

```c
int delimiter_was_quoted(char *filename)
{
    // Checks if the delimiter had quotes around it
    // Returns 1 if quoted, 0 if not quoted
    // Used to determine if variable expansion should occur
}
```

**Purpose:** Determines if here document delimiter was quoted to control variable expansion.

### `heredoc_subprocess(t_heredoc_sub heresub, char *delim, t_pipe_ctx *pipeline_ctx, char *temp_filename)` (static)
**Location:** `execute/pipeline_process_utils.c` (lines 37-end)

```c
static int heredoc_subprocess(t_heredoc_sub heresub, char *delim,
        t_pipe_ctx *pipeline_ctx, char *temp_filename)
{
    // Forks subprocess to handle here document input
    // Child process reads lines until delimiter
    // Parent waits for child completion
    // Returns exit status
}
```

**Purpose:** Creates subprocess to handle interactive here document input.

### `heredoc_content(int temp_fd, char *processed_delimiter, t_shell *shell, int should_expand)`
**Location:** `execute/heredoc_utils.c` (lines 70-95)

```c
int heredoc_content(int temp_fd, char *processed_delimiter,
        t_shell *shell, int should_expand)
{
    char    *line;
    size_t  delimiter_len;

    delimiter_len = ft_strlen(processed_delimiter);
    while (1)
    {
        if (!heredoc_rline(&line))                         // Read line from user
        {
            err_heredoc(processed_delimiter);              // Handle EOF
            break;
        }
        if (ft_strncmp(line, processed_delimiter, delimiter_len) == 0
            && ft_strlen(line) == delimiter_len)           // Found delimiter
        {
            free(line);
            break;
        }
        process_and_write_heredoc_line(temp_fd, line, shell, should_expand);
        free(line);
    }
    return (0);
}
```

**Purpose:** Reads here document content from user input until delimiter is found.

**Variable Expansion:** Conditionally expands variables based on delimiter quoting.

### `heredoc_rline(char **line)`
**Location:** `execute/heredoc_utils.c` (lines 44-48)

```c
int heredoc_rline(char **line)
{
    *line = readline("> ");                                // Read line with prompt
    return (*line != NULL);                                // Return success status
}
```

**Purpose:** Reads a single line from user input with here document prompt.

### `process_and_write_heredoc_line(int temp_fd, char *line, t_shell *shell, int should_expand)` (static)
**Location:** `execute/heredoc_utils.c` (lines 50-68)

```c
static void process_and_write_heredoc_line(int temp_fd, char *line,
        t_shell *shell, int should_expand)
{
    char *expanded_line;

    if (should_expand && shell)
    {
        expanded_line = expandvar(line, shell);            // Expand variables
        if (expanded_line)
        {
            write(temp_fd, expanded_line, ft_strlen(expanded_line));
            free(expanded_line);
        }
        else
            write(temp_fd, line, ft_strlen(line));         // Fallback to original
    }
    else
        write(temp_fd, line, ft_strlen(line));             // No expansion
    write(temp_fd, "\n", 1);                               // Add newline
}
```

**Purpose:** Processes and writes a single line of here document content with optional variable expansion.

## Variable Expansion System

### `expandvar(char *str, t_shell *shell)`
**Location:** `main/env_expand.c` (lines 82-97)

```c
char *expandvar(char *str, t_shell *shell)
{
    t_expand    holder;
    char        *tilde_expanded;

    if (!str)
        return (NULL);
    tilde_expanded = tilde(str, shell);                    // Expand tilde first
    if (!tilde_expanded)
        return (NULL);
    init_expand_holder(&holder, tilde_expanded);           // Initialize expansion context
    expander_loop(&holder, shell);                         // Perform expansion
    return (holder.result);
}
```

**Purpose:** Main variable expansion function that handles environment variables and special expansions.

### `init_expand_holder(t_expand *holder, char *tilde_expanded)`
**Location:** `main/env_expansion_utils.c` (lines 15-26)

```c
void init_expand_holder(t_expand *holder, char *tilde_expanded)
{
    // Initializes expansion context structure
    // Sets up result string and position trackers
    // Prepares for expansion processing
}
```

**Purpose:** Initializes the expansion context structure for variable processing.

### `expander_loop(t_expand *holder, t_shell *shell)`
**Location:** `main/env_expansion_utils.c` (lines 27-end)

```c
void expander_loop(t_expand *holder, t_shell *shell)
{
    // Main expansion loop that processes the string
    // Handles dollar signs and variable expansions
    // Manages quote states and special characters
}
```

**Purpose:** Main loop that processes string character by character for expansions.

### `expand_dollar(t_expand *holder, t_shell *shell)`
**Location:** `main/env_expand.c` (lines 58-81)

```c
int expand_dollar(t_expand *holder, t_shell *shell)
{
    if (holder->indx + 1 >= (int)ft_strlen(holder->result))
    {
        holder->indx++;
        return (1);
    }
    holder->var_start = &holder->result[holder->indx + 1];
    if (*holder->var_start == '"')
        return (expand_double_q(holder));                  // Handle quoted sections
    if (*holder->var_start == '\'')
        return (expand_single_q(holder));
    if (*holder->var_start == '?')
    {
        if (!handle_question_mark(holder, shell))          // Expand $?
            return (0);
        return (1);
    }
    if (*holder->var_start && (ft_isalpha(*holder->var_start)
            || *holder->var_start == '_'))
        return (handle_var_name_expansion(holder, shell)); // Expand variable name
    return (handle_special_chars(holder));
}
```

**Purpose:** Handles dollar sign expansions including variables, quotes, and special cases.

### Resource Cleanup

### `free_everything_in_child(char *delim, t_heredoc_sub heresub, t_pipe_ctx *pipeline_ctx)` (static)
**Location:** `execute/pipeline_process_utils.c` (lines 15-end)

```c
static void free_everything_in_child(char *delim, t_heredoc_sub heresub,
        t_pipe_ctx *pipeline_ctx)
{
    // Comprehensive cleanup function for child processes
    // Frees all allocated memory and closes file descriptors
    // Ensures no memory leaks in child processes
}
```

**Purpose:** Comprehensive cleanup function for child processes in here document handling.
4. **Status Collection:** Waits for all processes and determines final exit status
5. **Resource Cleanup:** Frees all allocated resources and closes file descriptors

## Resource Management and Cleanup

### `clean_pipe(t_split *commands, int **pipes, pid_t *pids, int cmd_count)`
**Location:** `execute/pipeline_utils.c` (lines 89-97)

```c
void clean_pipe(t_split *commands, int **pipes, pid_t *pids, int cmd_count)
{
    if (pipes)
        cleanup_pipes_safe(pipes, cmd_count);              // Safe pipe cleanup
    if (pids)
        free(pids);                                        // Free PID array
    if (commands)
        free(commands);                                    // Free command array
}
```

**Purpose:** Comprehensive cleanup function that frees all pipeline-related resources.

**Safety:** Uses safe cleanup functions to prevent double-free errors and handle partial allocations.

## Data Structures

### `t_pipe_ctx`
**Purpose:** Context structure that contains all pipeline execution data.

```c
typedef struct s_pipe_ctx
{
    t_split     *commands;      // Array of command segments
    int         **pipes;        // Array of pipe file descriptor pairs
    pid_t       *pids;          // Array of child process IDs
    int         cmd_count;      // Number of commands in pipeline
    t_shell     *shell;         // Shell state and environment
}   t_pipe_ctx;
```

**Usage:** Passed between pipeline functions to maintain execution state and resources.

## Key Features

### Combined System Management
**Dual Processing:** Handles both pipe creation and redirection setup
**Order Management:** Processes redirections before pipe connections
**Error Isolation:** Failures in one system don't crash the other
**Resource Coordination:** Shared cleanup between pipe and redirection systems

### Process Coordination
**Fork Strategy:** Creates one child process per command segment
**File Descriptor Management:** Proper setup of both pipes and redirections
**Signal Handling:** Coordinates signal delivery to all child processes

### Memory and Resource Safety
**Allocation Tracking:** Tracks all allocations for proper cleanup
**Partial Cleanup:** Handles cleanup when initialization fails partway
**File Descriptor Safety:** Ensures all opened fds are properly closed

## Integration Points

### With Redirection System
- Delegates single-command cases to `execute_with_redirections()`
- Handles here documents within pipeline context
- Manages redirection cleanup alongside pipe cleanup

### With Pipeline System
- Uses core pipeline infrastructure for process management
- Extends pipeline functionality with redirection support
- Maintains pipeline semantics for exit status handling

### With Shell Core
- Updates shell exit status based on pipeline result
- Inherits shell environment for child processes
- Maintains shell state consistency throughout execution

## Error Handling

### Allocation Failures
**Memory Errors:** Graceful handling of malloc failures at any stage
**Pipe Creation Errors:** Manages pipe() system call failures
**Partial Cleanup:** Cleans up successfully allocated resources on failure

### Runtime Errors
**Fork Failures:** Handles process creation failures gracefully
**Command Failures:** Properly reports and handles exec failures
**Signal Handling:** Manages interrupts and child process signals

### Resource Cleanup Strategy
**Hierarchical Cleanup:** Cleans up resources in reverse order of allocation
**Safe Functions:** Uses null-checking cleanup functions
**Error Propagation:** Returns appropriate error codes for different failure types

---

*This function represents the most complex execution path in minishell, combining the challenges of both pipeline and redirection management into a single, coordinated execution strategy.*
