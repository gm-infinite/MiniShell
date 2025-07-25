# MiniShell execute_args_array Function Documentation

This document provides detailed explanation of the `execute_args_array` function in the minishell project, which handles simple command execution without pipes or redirections.

## Function Overview

### `execute_args_array(t_split split, t_shell *shell)`
**Location:** `execute/execute.c`

**Purpose:** Executes simple commands without pipes or redirections, handling both built-in commands and external programs with proper environment management.

**Function Signature:**
```c
int execute_args_array(t_split split, t_shell *shell)
```

**Parameters:**
- **split:** Tokenized command without pipes or redirections
- **shell:** Shell state structure for environment and status management

**Return Value:** Exit status of the executed command

## Algorithm and Implementation

### Core Algorithm
```c
int execute_args_array(t_split split, t_shell *shell)
{
    char    **args;
    char    *command_path;
    int     exit_status;
    int     is_builtin;

    if (split.size == 0)                                   // Empty command
        return (0);
    
    args = convert_split_to_args(split);                   // Convert to argv format
    if (!args)
        return (1);                                        // Memory allocation failed
    
    is_builtin = check_builtin_command(args[0]);          // Check if built-in
    if (is_builtin)
    {
        exit_status = execute_builtin(args, shell);        // Execute built-in
        free_args_array(args);
        return (exit_status);
    }
    
    command_path = resolve_command_path(args[0], shell);   // Find executable
    if (!command_path)
    {
        print_command_not_found(args[0]);
        free_args_array(args);
        return (127);                                      // Command not found
    }
    
    exit_status = execute_external_command(command_path, args, shell);
    free(command_path);
    free_args_array(args);
    
    return (exit_status);
}
```

### Execution Flow
1. **Input Validation:** Checks for empty commands
2. **Argument Conversion:** Converts t_split to standard argv format
3. **Built-in Detection:** Determines if command is a shell built-in
4. **Path Resolution:** Finds executable path for external commands
5. **Command Execution:** Executes either built-in or external command
6. **Resource Cleanup:** Frees all allocated memory and resources

## Command Type Handling

### Built-in Commands
**Detection:** Checks command name against list of built-ins
**Execution:** Calls appropriate built-in function directly
**Built-ins Supported:**
- `echo` - Output text with optional formatting
- `cd` - Change directory
- `pwd` - Print working directory
- `export` - Set environment variables
- `unset` - Remove environment variables
- `env` - Display environment
- `exit` - Exit shell

### External Commands
**Path Resolution:** Uses PATH environment variable for command lookup
**Process Creation:** Forks child process for execution
**Environment Inheritance:** Passes shell environment to child process

## Implementation Details

### Argument Array Conversion
```c
char **convert_split_to_args(t_split split)
{
    char **args;
    int i;

    args = malloc(sizeof(char *) * (split.size + 1));      // +1 for NULL terminator
    if (!args)
        return (NULL);
    
    i = 0;
    while (i < split.size)
    {
        args[i] = ft_strdup(split.start[i]);               // Duplicate each argument
        if (!args[i])
        {
            free_partial_args(args, i);                    // Cleanup on failure
            return (NULL);
        }
        i++;
    }
    args[i] = NULL;                                        // NULL-terminate array
    
    return (args);
}
```

### Built-in Command Detection
```c
int check_builtin_command(char *command)
{
    static const char *builtins[] = {
        "echo", "cd", "pwd", "export", "unset", "env", "exit", NULL
    };
    int i;

    i = 0;
    while (builtins[i])
    {
        if (ft_strncmp(command, builtins[i], ft_strlen(builtins[i]) + 1) == 0)
            return (1);                                    // Found built-in
        i++;
    }
    return (0);                                            // Not a built-in
}
```

### Path Resolution
```c
char *resolve_command_path(char *command, t_shell *shell)
{
    char *path_env;
    char **path_dirs;
    char *full_path;
    int i;

    if (ft_strchr(command, '/'))                           // Absolute or relative path
        return (validate_and_dup_path(command));
    
    path_env = get_env_value("PATH", shell->env);          // Get PATH variable
    if (!path_env)
        return (NULL);
    
    path_dirs = ft_split(path_env, ':');                   // Split PATH by colons
    if (!path_dirs)
        return (NULL);
    
    i = 0;
    while (path_dirs[i])
    {
        full_path = construct_full_path(path_dirs[i], command);
        if (full_path && access(full_path, X_OK) == 0)     // Check if executable
        {
            free_string_array(path_dirs);
            return (full_path);                            // Found executable
        }
        free(full_path);
        i++;
    }
    
    free_string_array(path_dirs);
    return (NULL);                                         // Command not found
}
```

### External Command Execution
```c
int execute_external_command(char *command_path, char **args, t_shell *shell)
{
    pid_t pid;
    int status;
    char **env_array;

    env_array = convert_env_to_array(shell->env);          // Convert env to execve format
    if (!env_array)
        return (1);
    
    pid = fork();                                          // Create child process
    if (pid == -1)
    {
        free_string_array(env_array);
        return (1);                                        // Fork failed
    }
    
    if (pid == 0)                                          // Child process
    {
        execve(command_path, args, env_array);             // Execute command
        perror("execve");                                  // Should not reach here
        exit(127);
    }
    
    // Parent process
    waitpid(pid, &status, 0);                              // Wait for child
    free_string_array(env_array);
    
    if (WIFEXITED(status))
        return (WEXITSTATUS(status));                      // Normal exit
    else if (WIFSIGNALED(status))
        return (128 + WTERMSIG(status));                   // Killed by signal
    
    return (1);                                            // Unexpected termination
}
```

## Built-in Command Integration

### Command Dispatch
```c
int execute_builtin(char **args, t_shell *shell)
{
    char *command = args[0];
    
    if (ft_strncmp(command, "echo", 5) == 0)
        return (builtin_echo(args));
    else if (ft_strncmp(command, "cd", 3) == 0)
        return (builtin_cd(args, shell));
    else if (ft_strncmp(command, "pwd", 4) == 0)
        return (builtin_pwd());
    else if (ft_strncmp(command, "export", 7) == 0)
        return (builtin_export(args, shell));
    else if (ft_strncmp(command, "unset", 6) == 0)
        return (builtin_unset(args, shell));
    else if (ft_strncmp(command, "env", 4) == 0)
        return (builtin_env(shell));
    else if (ft_strncmp(command, "exit", 5) == 0)
        return (builtin_exit(args, shell));
    
    return (1);                                            // Unknown built-in
}
```

### Environment Management
**Variable Access:** Provides access to shell environment variables
**Variable Modification:** Allows built-ins to modify environment
**State Consistency:** Maintains shell state across built-in executions

## Key Features

### Memory Management
**Argument Duplication:** Creates independent copies of arguments
**Environment Conversion:** Converts environment for execve compatibility
**Cleanup Tracking:** Ensures all allocated memory is freed

### Error Handling
**Command Not Found:** Provides clear error messages for missing commands
**Permission Errors:** Handles execute permission failures
**Memory Failures:** Gracefully handles allocation failures

### Signal Handling
**Child Process Signals:** Properly handles child process termination signals
**Signal Propagation:** Ensures signals are handled correctly
**Exit Status:** Converts signal termination to appropriate exit codes

## Performance Considerations

### Process Optimization
**Built-in Efficiency:** Built-ins execute without forking
**Path Caching:** Could implement command path caching for repeated use
**Environment Optimization:** Efficient environment variable access

### Memory Efficiency
**Stack Usage:** Uses stack variables where possible
**Dynamic Allocation:** Only allocates memory when necessary
**Early Cleanup:** Frees memory as soon as it's no longer needed

### System Call Optimization
**Minimal System Calls:** Reduces system call overhead where possible
**Efficient Waiting:** Uses appropriate wait strategies
**File Descriptor Management:** Minimizes fd operations

## Error Recovery

### Command Execution Errors
**File Not Found:** Clear error messages for missing executables
**Permission Denied:** Handles execute permission errors
**Memory Errors:** Graceful handling of allocation failures

### Process Management Errors
**Fork Failures:** Handles process creation failures
**Exec Failures:** Proper error reporting for exec failures
**Wait Failures:** Manages child process waiting errors

### Resource Cleanup
**Partial Cleanup:** Cleans up successfully allocated resources on failure
**Signal Safety:** Cleanup functions are signal-safe
**Memory Safety:** Prevents memory leaks in all error paths

## Integration with Shell Systems

### Environment Synchronization
**Variable Updates:** Keeps environment synchronized with built-in changes
**Path Management:** Maintains PATH for command resolution
**Working Directory:** Keeps working directory consistent

### Status Management
**Exit Status Tracking:** Updates shell's last exit status
**Error Propagation:** Proper error code propagation
**State Consistency:** Maintains shell state integrity

### Built-in Coordination
**State Sharing:** Provides shell state access to built-ins
**Environment Access:** Allows built-ins to access and modify environment
**Error Reporting:** Consistent error reporting across built-ins

---

*This function serves as the foundation of command execution in minishell, providing the basic execution mechanism that other more complex execution functions build upon.*
