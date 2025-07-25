# MiniShell Pipeline Execution Documentation

This document provides detailed explanations of pipeline execution functions in the minishell project, focusing on pipe handling and complex command execution with both pipes and redirections.

## Pipeline System Overview

The pipeline system in minishell handles:
- **Simple Pipelines:** Commands connected with pipe operators (`|`)
- **Complex Pipelines:** Commands with both pipes and redirections
- **Process Management:** Fork/exec coordination and cleanup
- **File Descriptor Management:** Proper pipe and redirection handling

## Core Pipeline Functions

### `execute_pipeline(t_split split, t_shell *shell)`
**Location:** `execute/pipeline_utils.c`

**Purpose:** Executes commands connected by pipe operators without additional redirections.

**Process Flow:**
1. **Pipeline Setup:** Creates pipe file descriptors for command chain
2. **Process Creation:** Forks child processes for each command segment
3. **File Descriptor Management:** Connects stdout of each command to stdin of next
4. **Process Coordination:** Waits for all processes and collects exit status
5. **Cleanup:** Closes all pipe file descriptors and cleans up resources

**Algorithm:**
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

**Key Features:**
- **Memory Management:** Allocates and manages arrays for pipes and process IDs
- **Signal Handling:** Proper signal propagation to child processes
- **Error Handling:** Graceful cleanup on allocation or execution failures
- **Exit Status:** Returns exit status of last command in pipeline

### `execute_pipe_redir(t_split split, t_shell *shell)`
**Location:** `execute/pipeline_utils.c`

**Purpose:** Executes commands with both pipe operators and redirection operators.

**Complexity Handling:**
1. **Dual Processing:** Handles both pipe creation and redirection setup
2. **Order Management:** Processes redirections before pipe connections
3. **File Descriptor Coordination:** Manages complex fd routing
4. **Error Propagation:** Maintains error handling for both systems

**Algorithm:**
```c
int execute_pipe_redir(t_split split, t_shell *shell)
{
    t_complex_pipeline  complex_data;
    int                 exit_status;
    int                 pipe_count;

    pipe_count = count_pipes(split);
    init_complex_pipeline(&complex_data, pipe_count, split);
    
    if (!validate_complex_syntax(split))              // Validate combined syntax
        return (cleanup_complex_pipeline(&complex_data, 2));
    
    process_complex_commands(&complex_data, split, shell);
    exit_status = wait_for_complex_pipeline(&complex_data);
    cleanup_complex_pipeline(&complex_data, 0);
    
    return (exit_status);
}
```

**Integration Challenges:**
- **Precedence Rules:** Redirections are applied before pipe connections
- **File Descriptor Conflicts:** Ensures redirections don't interfere with pipes
- **Error Isolation:** Failures in one system don't crash the other
- **Resource Coordination:** Shared cleanup between pipe and redirection systems

## Pipeline Data Structures

### `t_pipeline_data`
**Purpose:** Manages pipeline execution state and resources.

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

**Resource Management:**
- **Dynamic Allocation:** Sizes based on actual pipe count
- **Cleanup Tracking:** Enables proper resource deallocation
- **State Tracking:** Maintains current position in pipeline processing

### `t_complex_pipeline`
**Purpose:** Manages combined pipe and redirection execution.

```c
typedef struct s_complex_pipeline
{
    t_pipeline_data pipeline;      // Base pipeline functionality
    t_redir_data    *redirections; // Per-command redirection data
    int             has_heredoc;   // Tracks here document presence
}   t_complex_pipeline;
```

## Pipeline Process Management

### Process Creation Strategy
**Fork Coordination:** Each command runs in separate child process
**File Descriptor Inheritance:** Proper fd setup before exec calls
**Signal Handling:** Parent coordinates signal delivery to child processes

### Exit Status Collection
**Sequential Waiting:** Waits for processes in creation order
**Status Propagation:** Returns exit status of final command
**Error Aggregation:** Handles partial pipeline failures

### Resource Cleanup
**Pipe Closure:** Systematically closes all pipe file descriptors
**Process Cleanup:** Ensures all child processes are reaped
**Memory Deallocation:** Frees all dynamically allocated structures

## Error Handling and Recovery

### Syntax Validation
- **Pipe Placement:** Validates pipe operators are not at command boundaries
- **Command Completeness:** Ensures each pipe segment has a command
- **Redirection Conflicts:** Checks for conflicting redirection and pipe usage

### Runtime Error Handling
- **Fork Failures:** Handles process creation failures gracefully
- **Pipe Creation Failures:** Manages pipe() system call failures
- **Exec Failures:** Properly reports command not found errors

### Cleanup Procedures
- **Partial Cleanup:** Cleans up successfully allocated resources on failure
- **Signal Safety:** Cleanup functions are signal-safe
- **Memory Safety:** Prevents memory leaks in error conditions

## Performance Considerations

### Process Optimization
- **Minimal Forking:** Only creates necessary child processes
- **Efficient Waiting:** Uses optimal wait strategies for process collection
- **Resource Reuse:** Reuses file descriptors where possible

### Memory Management
- **Stack Allocation:** Uses stack variables for temporary data
- **Heap Minimization:** Minimizes dynamic allocation for performance
- **Early Cleanup:** Releases resources as soon as they're no longer needed

## Integration with Shell Systems

### Environment Management
- **Variable Inheritance:** Child processes inherit shell environment
- **Path Resolution:** Uses shell's PATH for command lookup
- **Working Directory:** Maintains consistent working directory across pipeline

### Signal Coordination
- **Interrupt Handling:** Properly handles Ctrl+C during pipeline execution
- **Process Group Management:** Manages process groups for signal delivery
- **Cleanup Signals:** Uses signals for coordinated cleanup

---

*For specific implementation details, refer to source files in `execute/` directory.*
