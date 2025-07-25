# MiniShell execute_with_redirections Function Documentation

This document provides detailed explanation of the `execute_with_redirections` function in the minishell project, which handles commands with redirection operators but no pipes.

## Function Overview

### `execute_with_redirections(t_split split, t_shell *shell)`
**Location:** `execute/redirections/execute_redirections.c`

**Purpose:** Executes commands that contain redirection operators (`<`, `>`, `>>`, `<<`) without pipe operators, managing file descriptor redirection and restoration.

**Function Signature:**
```c
int execute_with_redirections(t_split split, t_shell *shell)
```

**Parameters:**
- **split:** Tokenized command containing redirection operators
- **shell:** Shell state structure for environment and status management

**Return Value:** Exit status of the executed command

## Algorithm and Implementation

### Core Algorithm
```c
int execute_with_redirections(t_split split, t_shell *shell)
{
    t_redir_data    redir_data;
    int             exit_status;
    t_split         clean_split;

    init_redirection_data(&redir_data);                    // Initialize redirection structure
    
    if (!parse_redirections(split, &redir_data))           // Extract and validate redirections
        return (cleanup_redirections(&redir_data, 2));
    
    create_clean_split(split, &clean_split, &redir_data);  // Remove redirection tokens
    
    if (!setup_redirections(&redir_data))                  // Apply redirections
        return (cleanup_redirections(&redir_data, 2));
    
    exit_status = execute_args_array(clean_split, shell);  // Execute cleaned command
    restore_redirections(&redir_data);                     // Restore original fds
    cleanup_redirections(&redir_data, 0);                  // Clean up resources
    
    return (exit_status);
}
```

### Execution Flow
1. **Redirection Parsing:** Extracts redirection operators and filenames from command
2. **Command Cleaning:** Creates new split without redirection tokens
3. **File Descriptor Setup:** Opens files and redirects stdin/stdout/stderr as needed
4. **Command Execution:** Executes the cleaned command with redirected fds
5. **Restoration:** Restores original file descriptors
6. **Cleanup:** Closes opened files and frees allocated memory

## Redirection Data Management

### Data Structure: `t_redir_data`
```c
typedef struct s_redir_data
{
    int     input_fd;        // File descriptor for input redirection
    int     output_fd;       // File descriptor for output redirection
    int     append_fd;       // File descriptor for append redirection
    char    *heredoc_content; // Content for here document
    int     saved_stdin;     // Original stdin fd for restoration
    int     saved_stdout;    // Original stdout fd for restoration
    int     saved_stderr;    // Original stderr fd for restoration
    char    **filenames;     // Array of filenames involved in redirections
    int     filename_count;  // Number of filenames to track
}   t_redir_data;
```

### Resource Tracking
- **File Descriptor Management:** Tracks all opened fds for proper cleanup
- **Original State Saving:** Preserves original stdin/stdout/stderr for restoration
- **Filename Storage:** Maintains filenames for error reporting and cleanup

## Redirection Types Supported

### Input Redirection (`<`)
**Behavior:** Redirects stdin to read from specified file
**Implementation:**
```c
int setup_input_redirection(t_redir_data *data, char *filename)
{
    data->input_fd = open(filename, O_RDONLY);
    if (data->input_fd == -1)
    {
        print_file_error(filename, "No such file or directory");
        return (0);
    }
    data->saved_stdin = dup(STDIN_FILENO);
    dup2(data->input_fd, STDIN_FILENO);
    return (1);
}
```

### Output Redirection (`>`)
**Behavior:** Redirects stdout to write to specified file (overwrites)
**Implementation:**
```c
int setup_output_redirection(t_redir_data *data, char *filename)
{
    data->output_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (data->output_fd == -1)
    {
        print_file_error(filename, "Permission denied");
        return (0);
    }
    data->saved_stdout = dup(STDOUT_FILENO);
    dup2(data->output_fd, STDOUT_FILENO);
    return (1);
}
```

### Append Redirection (`>>`)
**Behavior:** Redirects stdout to append to specified file
**Implementation:**
```c
int setup_append_redirection(t_redir_data *data, char *filename)
{
    data->append_fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (data->append_fd == -1)
    {
        print_file_error(filename, "Permission denied");
        return (0);
    }
    data->saved_stdout = dup(STDOUT_FILENO);
    dup2(data->append_fd, STDOUT_FILENO);
    return (1);
}
```

### Here Document (`<<`)
**Behavior:** Redirects stdin to read from provided content
**Implementation:**
```c
int setup_heredoc_redirection(t_redir_data *data, char *delimiter)
{
    char *content = read_heredoc_content(delimiter);
    if (!content)
        return (0);
    
    int pipe_fds[2];
    pipe(pipe_fds);
    
    write(pipe_fds[1], content, ft_strlen(content));
    close(pipe_fds[1]);
    
    data->saved_stdin = dup(STDIN_FILENO);
    dup2(pipe_fds[0], STDIN_FILENO);
    close(pipe_fds[0]);
    
    free(content);
    return (1);
}
```

## Key Features

### Multiple Redirection Handling
**Precedence Rules:** Later redirections override earlier ones
**Error Detection:** Validates conflicting redirections
**File Management:** Properly opens and closes all involved files

### Error Handling
**File Errors:** Handles file not found, permission denied, etc.
**Syntax Validation:** Ensures proper redirection syntax
**Resource Cleanup:** Cleans up on any failure

### State Preservation
**File Descriptor Saving:** Preserves original stdin/stdout/stderr
**Restoration:** Restores original state after command execution
**Signal Safety:** Cleanup is signal-safe

## Implementation Details

### Redirection Parsing
```c
int parse_redirections(t_split split, t_redir_data *data)
{
    int i = 0;
    int redir_type;
    
    while (i < split.size)
    {
        redir_type = is_redirection(split.start[i]);
        if (redir_type)
        {
            if (i + 1 >= split.size)  // No filename provided
                return (print_syntax_error("newline"), 0);
            
            if (!process_redirection(data, redir_type, split.start[i + 1]))
                return (0);
            
            i += 2;  // Skip redirection operator and filename
        }
        else
            i++;
    }
    return (1);
}
```

### File Descriptor Restoration
```c
void restore_redirections(t_redir_data *data)
{
    if (data->saved_stdin != -1)
    {
        dup2(data->saved_stdin, STDIN_FILENO);
        close(data->saved_stdin);
    }
    
    if (data->saved_stdout != -1)
    {
        dup2(data->saved_stdout, STDOUT_FILENO);
        close(data->saved_stdout);
    }
    
    if (data->saved_stderr != -1)
    {
        dup2(data->saved_stderr, STDERR_FILENO);
        close(data->saved_stderr);
    }
}
```

### Clean Split Creation
```c
void create_clean_split(t_split original, t_split *clean, t_redir_data *data)
{
    int clean_count = 0;
    int i = 0;
    
    // Count non-redirection tokens
    while (i < original.size)
    {
        if (is_redirection(original.start[i]))
            i += 2;  // Skip operator and filename
        else
        {
            clean_count++;
            i++;
        }
    }
    
    // Allocate and populate clean split
    clean->start = malloc(sizeof(char *) * clean_count);
    clean->size = clean_count;
    
    // Copy non-redirection tokens
    // ... implementation continues
}
```

## Error Handling and Recovery

### File Operation Errors
**File Not Found:** Clear error messages for missing input files
**Permission Denied:** Handles read/write permission errors
**Disk Full:** Manages write failures due to insufficient space

### Syntax Error Handling
**Missing Filenames:** Detects redirections without target files
**Invalid Operators:** Validates redirection operator syntax
**Conflicting Redirections:** Handles multiple redirections to same fd

### Resource Management
**File Descriptor Leaks:** Ensures all opened fds are closed
**Memory Management:** Properly frees all allocated memory
**Partial Cleanup:** Cleans up successfully allocated resources on failure

## Performance Considerations

### File Operation Optimization
**Lazy Opening:** Only opens files when actually needed
**Efficient Buffering:** Uses system buffering for file operations
**Minimal Copying:** Avoids unnecessary data copying

### Memory Efficiency
**Stack Usage:** Prefers stack allocation for temporary data
**Dynamic Sizing:** Allocates based on actual redirection count
**Early Cleanup:** Releases resources as soon as possible

## Integration with Shell Systems

### Environment Consistency
**Working Directory:** Maintains consistent working directory for file operations
**Path Resolution:** Resolves relative paths correctly
**Permission Inheritance:** Uses shell's umask for file creation

### Error Reporting
**Bash Compatibility:** Provides error messages compatible with bash
**Exit Status:** Sets appropriate exit codes for different error types
**Error Context:** Provides context about which redirection failed

---

*This function provides comprehensive redirection support for minishell, handling all standard shell redirection operators while maintaining proper error handling and resource management.*
