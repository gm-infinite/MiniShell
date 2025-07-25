# MiniShell execute_pipeline Function Documentation

This document provides detailed explanation of the `execute_pipeline` function in the minishell project, which handles commands connected by pipe operators.

## Function Overview

### `execute_pipeline(t_split split, t_shell *shell)`
**Location:** `execute/pipeline_utils.c`

**Purpose:** Executes commands connected by pipe operators without additional redirections, creating a pipeline where the output of each command becomes the input of the next.

**Function Signature:**
```c
int execute_pipeline(t_split split, t_shell *shell)
```

**Parameters:**
- **split:** Tokenized command containing pipe operators
- **shell:** Shell state structure for environment and status management

**Return Value:** Exit status of the last command in the pipeline

## Algorithm and Implementation

### Core Algorithm
```c
int execute_pipeline(t_split split, t_shell *shell)
{
    t_pipeline_data    pipeline_data;
    int                exit_status;
    int                pipe_count;

    pipe_count = count_pipes(split);                    // Count number of pipes
    if (pipe_count == 0)                               // No pipes found
        return (execute_args_array(split, shell));     // Execute as single command
    
    init_pipeline_data(&pipeline_data, pipe_count);    // Initialize pipeline structure
    if (!pipeline_data.pipes || !pipeline_data.pids)   // Memory allocation failed
        return (cleanup_pipeline(&pipeline_data, 1));
    
    setup_pipeline_processes(&pipeline_data, split, shell);  // Create all processes
    exit_status = wait_for_pipeline(&pipeline_data);         // Wait for completion
    cleanup_pipeline(&pipeline_data, 0);                     // Clean up resources
    
    return (exit_status);
}
```

### Execution Flow
1. **Pipe Counting:** Determines the number of pipe operators in the command
2. **Resource Allocation:** Creates arrays for pipe file descriptors and process IDs
3. **Process Creation:** Forks child processes for each command segment
4. **File Descriptor Management:** Connects stdout of each command to stdin of next
5. **Process Coordination:** Waits for all processes and collects exit status
6. **Resource Cleanup:** Closes all pipe file descriptors and frees memory

## Pipeline Data Management

### Data Structure: `t_pipeline_data`
```c
typedef struct s_pipeline_data
{
    int     **pipes;        // Array of pipe file descriptor pairs
    pid_t   *pids;          // Array of child process IDs
    int     pipe_count;     // Number of pipes in the pipeline
    int     cmd_count;      // Number of commands (pipe_count + 1)
    int     current_cmd;    // Currently processing command index
}   t_pipeline_data;
```

### Resource Management
- **Dynamic Allocation:** Sizes arrays based on actual pipe count
- **Cleanup Tracking:** Enables proper resource deallocation
- **State Tracking:** Maintains current position in pipeline processing

## Key Features

### Process Management
**Fork Strategy:** Creates one child process per command
**File Descriptor Inheritance:** Each child inherits appropriate pipe fds
**Signal Handling:** Parent coordinates signal delivery to child processes

### Pipe Connection Logic
**Sequential Connection:** Command N's stdout connects to Command N+1's stdin
**File Descriptor Duplication:** Uses dup2() for proper fd redirection
**Cleanup Order:** Closes unused pipe ends in correct sequence

### Exit Status Handling
**Last Command Priority:** Returns exit status of final command in pipeline
**Process Waiting:** Waits for all processes to prevent zombies
**Error Propagation:** Handles individual command failures appropriately

## Implementation Details

### Memory Allocation
```c
void init_pipeline_data(t_pipeline_data *data, int pipe_count)
{
    data->pipe_count = pipe_count;
    data->cmd_count = pipe_count + 1;
    data->current_cmd = 0;
    
    // Allocate array of pipe fd pairs
    data->pipes = malloc(sizeof(int *) * pipe_count);
    for (int i = 0; i < pipe_count; i++)
        data->pipes[i] = malloc(sizeof(int) * 2);
    
    // Allocate array of process IDs
    data->pids = malloc(sizeof(pid_t) * data->cmd_count);
}
```

### Process Creation Strategy
```c
void setup_pipeline_processes(t_pipeline_data *data, t_split split, t_shell *shell)
{
    int cmd_start = 0;
    
    // Create pipes first
    for (int i = 0; i < data->pipe_count; i++)
        pipe(data->pipes[i]);
    
    // Fork processes for each command
    for (int i = 0; i < data->cmd_count; i++)
    {
        data->pids[i] = fork();
        if (data->pids[i] == 0)  // Child process
        {
            setup_child_fds(data, i);           // Connect pipes
            execute_command_segment(split, cmd_start, shell);  // Execute command
        }
        cmd_start = find_next_pipe(split, cmd_start) + 1;
    }
}
```

### File Descriptor Management
**Child Process Setup:**
- First command: stdin unchanged, stdout to pipe[0][1]
- Middle commands: stdin from pipe[i-1][0], stdout to pipe[i][1]
- Last command: stdin from pipe[n-1][0], stdout unchanged

**Parent Process Cleanup:**
- Closes all pipe file descriptors after fork
- Maintains no open fds to prevent hanging

## Error Handling

### Allocation Failures
**Memory Errors:** Gracefully handles malloc failures
**Pipe Creation Errors:** Manages pipe() system call failures
**Partial Cleanup:** Cleans up successfully allocated resources

### Runtime Errors
**Fork Failures:** Handles process creation failures
**Exec Failures:** Reports command not found errors
**Signal Handling:** Properly handles interrupts during execution

### Resource Cleanup
```c
void cleanup_pipeline(t_pipeline_data *data, int error_code)
{
    // Close all pipe file descriptors
    for (int i = 0; i < data->pipe_count; i++)
    {
        if (data->pipes[i])
        {
            close(data->pipes[i][0]);
            close(data->pipes[i][1]);
            free(data->pipes[i]);
        }
    }
    
    // Free memory
    if (data->pipes)
        free(data->pipes);
    if (data->pids)
        free(data->pids);
}
```

## Performance Considerations

### Process Optimization
**Minimal Forking:** Only creates necessary child processes
**Efficient Waiting:** Uses optimal wait strategies for process collection
**Resource Reuse:** Reuses file descriptors where possible

### Memory Efficiency
**Stack Variables:** Uses stack allocation for temporary data
**Dynamic Sizing:** Allocates based on actual pipeline length
**Early Cleanup:** Releases resources as soon as they're no longer needed

## Integration with Shell Systems

### Environment Inheritance
**Variable Propagation:** Child processes inherit shell environment
**Path Resolution:** Uses shell's PATH for command lookup
**Working Directory:** Maintains consistent working directory

### Signal Coordination
**Interrupt Handling:** Properly handles Ctrl+C during pipeline execution
**Process Group Management:** Manages process groups for signal delivery
**Cleanup Signals:** Uses signals for coordinated cleanup

---

*This function implements the core pipeline functionality of minishell, providing the foundation for complex command chaining and data flow between processes.*
