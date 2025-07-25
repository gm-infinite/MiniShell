# MiniShell Quote Processing System Documentation

This document provides detailed explanation of the quote processing system in the minishell project, covering quote validation, removal, and variable expansion within quoted contexts.

## Quote Processing System Overview

The quote processing system in minishell handles:
- **Quote Validation:** Ensures proper quote pairing and nesting
- **Quote Removal:** Removes quotes while preserving content integrity
- **Variable Expansion:** Expands variables within appropriate quote contexts
- **Escaped Characters:** Handles escaped quotes and special characters
- **State Management:** Tracks quote states during processing

## Core Quote Processing Functions

### `check_quotes(char *str)`
**Location:** `parser/quotes.c` (lines 16-34)

```c
int check_quotes(char *str)
{
    int i;
    int single_open;
    int double_open;

    if (!str)
        return (1);                                        // NULL string is valid
    
    i = -1;
    single_open = 0;
    double_open = 0;
    while (str[++i])
    {
        if (str[i] == '\'' && !double_open)                // Single quote outside double quotes
            single_open = !single_open;
        else if (str[i] == '"' && !single_open)            // Double quote outside single quotes
            double_open = !double_open;
    }
    return (single_open == 0 && double_open == 0);        // Both must be closed
}
```

**Purpose:** Validates that all quotes in a string are properly paired and nested.

**Algorithm:**
1. **State Tracking:** Maintains separate counters for single and double quote states
2. **Nesting Rules:** Single quotes inside double quotes are ignored and vice versa
3. **Balance Check:** Returns true only if both quote types are properly closed

**Return Value:** 1 if quotes are balanced, 0 if unbalanced

### `process_quotes(char *str, t_shell *shell)`
**Location:** `parser/quotes.c` (lines 36-54)

```c
char *process_quotes(char *str, t_shell *shell)
{
    char *result;
    char *expanded;

    if (!str)
        return (NULL);
    
    expanded = expandvar_q(str, shell);                    // Expand variables with quote awareness
    if (!expanded)
        return (NULL);
    
    result = remove_quotes_from_string(expanded);          // Remove quotes from expanded string
    if (!result)
        result = expanded;                                 // Use expanded if quote removal fails
    else
    {
        if (expanded != str)                               // Free intermediate result
            free(expanded);
    }
    return (result);
}
```

**Purpose:** Main quote processing function that combines variable expansion and quote removal.

**Process Flow:**
1. **Variable Expansion:** Expands variables while respecting quote contexts
2. **Quote Removal:** Removes quote characters while preserving content
3. **Memory Management:** Properly manages intermediate allocations
4. **Fallback Handling:** Uses expanded string if quote removal fails

### `process_args_quotes(char **args, t_shell *shell)`
**Location:** `parser/quotes.c` (lines 56-75)

```c
void process_args_quotes(char **args, t_shell *shell)
{
    int     i;
    char    *processed;

    if (!args)
        return;
    
    i = -1;
    while (args[++i])
    {
        processed = process_quotes(args[i], shell);        // Process each argument
        if (processed)
        {
            free(args[i]);                                 // Free original
            args[i] = processed;                           // Replace with processed
        }
    }
}
```

**Purpose:** Processes quotes for an entire array of command arguments.

**In-Place Processing:** Modifies the argument array in place, replacing each argument with its processed version.

## Quote Removal System

### `remove_quotes_from_string(char *str)`
**Location:** `parser/quotes_utils.c` (lines 22-36)

```c
char *remove_quotes_from_string(char *str)
{
    char *result;
    int  indices[2];
    int  states[2];

    result = malloc(ft_strlen(str) * 2 + 1);               // Allocate generous buffer
    if (!result)
        return (NULL);
    
    init_quote_removal(&indices[0], &indices[1], states); // Initialize state tracking
    
    while (str[indices[0]])
        if (process_quote_character(str, result, indices, states))
            continue;                                      // Skip to next iteration
    
    result[indices[1]] = '\0';                             // Null terminate
    return (result);
}
```

**Purpose:** Removes quote characters from a string while preserving the content inside quotes.

**Buffer Allocation:** Allocates double the original size to handle potential expansion during processing.

**State Management:** Uses arrays to track current positions and quote states.

### `init_quote_removal(int *i, int *j, int *states)` (static)
**Location:** `parser/quotes_utils.c` (lines 15-21)

```c
static void init_quote_removal(int *i, int *j, int *states)
{
    *i = 0;                                                // Input string index
    *j = 0;                                                // Output string index
    states[0] = 0;                                         // Single quote state
    states[1] = 0;                                         // Double quote state
}
```

**Purpose:** Initializes all state variables for quote removal processing.

**State Variables:**
- **i:** Current position in input string
- **j:** Current position in output string
- **states[0]:** Single quote state (0=outside, 1=inside)
- **states[1]:** Double quote state (0=outside, 1=inside)

## Quote Character Processing

### `process_quote_character(char *str, char *result, int *indices, int *states)`
**Location:** `parser/quotes_processing_utils.c` (lines 46-66)

```c
int process_quote_character(char *str, char *result, int *indices, int *states)
{
    int i;
    int j;

    i = indices[0];
    j = indices[1];
    
    if (handle_escaped_quote(str, result, i, j))           // Handle escaped quotes
    {
        indices[1] = j + 1;
        indices[0] = i + 2;                                // Skip escape sequence
        return (1);
    }
    
    if (should_copy_character(str[i], states))             // Copy non-quote characters
    {
        result[j] = str[i];
        indices[1] = j + 1;
    }
    
    update_quote_states(str[i], states);                   // Update quote state tracking
    indices[0] = i + 1;
    return (0);
}
```

**Purpose:** Processes a single character during quote removal, handling state updates and character copying.

**Return Value:** 1 if special handling occurred (continue loop), 0 for normal processing.

### `handle_escaped_quote(char *str, char *result, int i, int j)`
**Location:** `parser/quotes_processing_utils.c` (lines 15-23)

```c
int handle_escaped_quote(char *str, char *result, int i, int j)
{
    if (str[i] == '\001' && str[i + 1] == '\'')            // Escaped single quote
    {
        result[j] = '\'';                                  // Copy literal quote
        return (1);
    }
    return (0);
}
```

**Purpose:** Handles escaped quote characters that should be preserved as literal quotes.

**Escape Sequence:** Uses `\001` as an internal escape marker for quotes.

### `update_quote_states(char c, int *states)`
**Location:** `parser/quotes_processing_utils.c` (lines 25-32)

```c
void update_quote_states(char c, int *states)
{
    if (c == '\'' && !states[1])                          // Single quote outside double quotes
        states[0] = !states[0];
    else if (c == '"' && !states[0])                      // Double quote outside single quotes
        states[1] = !states[1];
}
```

**Purpose:** Updates quote state tracking based on encountered quote characters.

**Nesting Rules:** Only toggles quote state if not inside the other quote type.

### `should_copy_character(char c, int *states)`
**Location:** `parser/quotes_processing_utils.c` (lines 34-43)

```c
int should_copy_character(char c, int *states)
{
    if (c == '\'' && !states[1])                          // Single quote outside double quotes
        return (0);                                        // Don't copy quote character
    if (c == '"' && !states[0])                           // Double quote outside single quotes
        return (0);                                        // Don't copy quote character
    return (1);                                            // Copy all other characters
}
```

**Purpose:** Determines whether a character should be copied to the output string.

**Quote Filtering:** Filters out quote characters that are acting as delimiters.

## Advanced Quote Processing with Expansion

### `process_q_expand(char *str, t_shell *shell)`
**Location:** `execute/redirections/redir_quote_utils.c` (lines 72-85)

```c
char *process_q_expand(char *str, t_shell *shell)
{
    char *result;
    int  vars[4];

    if (!str)
        return (NULL);
    
    result = ft_calloc(1, 1);                              // Initialize empty result
    if (!result)
        return (NULL);
    
    init_expansion_vars(vars);                             // Initialize state variables
    return (process_quote_expansion_loop(str, shell, result, vars));
}
```

**Purpose:** Advanced quote processing that handles both expansion and quote removal simultaneously.

**State Management:** Uses array of 4 integers to track processing state.

### `init_expansion_vars(int *vars)` (static)
**Location:** `execute/redirections/redir_quote_utils.c` (lines 15-21)

```c
static void init_expansion_vars(int *vars)
{
    vars[0] = 0;                                           // Current position
    vars[1] = 0;                                           // Segment start
    vars[2] = 0;                                           // Single quote state
    vars[3] = 0;                                           // Double quote state
}
```

**Purpose:** Initializes state variables for advanced quote and expansion processing.

### `process_quote_expansion_loop(char *str, t_shell *shell, char *result, int *vars)` (static)
**Location:** `execute/redirections/redir_quote_utils.c` (lines 52-70)

```c
static char *process_quote_expansion_loop(char *str, t_shell *shell,
        char *result, int *vars)
{
    char *temp_result;

    while (str[vars[0]])
    {
        vars[1] = vars[0];                                 // Mark segment start
        temp_result = process_quotes_and_expansions(str, result, vars);
        if (temp_result)
        {
            result = temp_result;
            continue;
        }
        
        while (str[vars[0]] && ((str[vars[0]] != '\'' || vars[3])
                && (str[vars[0]] != '"' || vars[2])))     // Skip to next quote
            vars[0]++;
        
        if (vars[0] > vars[1])                             // Process segment
        {
            result = handle_segment_processing(result, str, vars, shell);
            if (!result)
                return (NULL);
        }
    }
    return (result);
}
```

**Purpose:** Main processing loop that handles segments between quotes and processes expansions.

**Segment Processing:** Identifies and processes text segments between quote boundaries.

### `process_quotes_and_expansions(char *str, char *result, int *vars)` (static)
**Location:** `execute/redirections/redir_quote_utils.c` (lines 34-50)

```c
static char *process_quotes_and_expansions(char *str, char *result, int *vars)
{
    char *temp_result;

    if (str[vars[0]] == '\'')
    {
        temp_result = handle_single_quote_case(result, &vars[0],
                &vars[2], vars[3]);                       // Handle single quotes
        if (temp_result)
            return (temp_result);
    }
    else if (str[vars[0]] == '"')
    {
        temp_result = handle_double_quote_case(result, &vars[0],
                &vars[3], vars[2]);                       // Handle double quotes
        if (temp_result)
            return (temp_result);
    }
    return (NULL);
}
```

**Purpose:** Processes quote characters and determines appropriate handling based on quote type and context.

### `handle_single_quote_case(char *result, int *i, int *in_single_quotes, int in_double_quotes)` (static)
**Location:** `execute/redirections/redir_quote_utils.c` (lines 23-32)

```c
static char *handle_single_quote_case(char *result, int *i,
        int *in_single_quotes, int in_double_quotes)
{
    if (!in_double_quotes)                                 // Not inside double quotes
    {
        result = handle_quote_char(result, '\'');          // Process single quote
        *in_single_quotes = !*in_single_quotes;           // Toggle state
        (*i)++;
        return (result);
    }
    return (NULL);                                         // Inside double quotes, ignore
}
```

**Purpose:** Handles single quote characters with proper state management.

**Context Awareness:** Only processes single quotes when not inside double quotes.

## Integration with Variable Expansion

The quote processing system integrates closely with the variable expansion system:

### Variable Expansion Rules
- **Single Quotes:** No variable expansion occurs inside single quotes
- **Double Quotes:** Variable expansion occurs inside double quotes
- **Unquoted:** Variable expansion occurs in unquoted text
- **Escaped Variables:** `\$` prevents variable expansion

### Processing Order
1. **Quote Validation:** Ensures quotes are properly balanced
2. **Variable Expansion:** Expands variables according to quote context
3. **Quote Removal:** Removes quote delimiters while preserving content
4. **Result Finalization:** Returns processed string ready for execution

## Error Handling and Edge Cases

### Memory Management
- **Allocation Failures:** Graceful handling of malloc failures
- **Buffer Sizing:** Generous buffer allocation to prevent overflows
- **Cleanup:** Proper cleanup of intermediate allocations

### Quote Edge Cases
- **Nested Quotes:** Proper handling of quotes inside other quotes
- **Escaped Quotes:** Special handling for literal quote characters
- **Empty Quotes:** Correct processing of empty quoted strings
- **Unclosed Quotes:** Detection and handling of unbalanced quotes

### Integration Safety
- **State Consistency:** Maintains consistent state across all processing functions
- **Input Validation:** Validates input parameters before processing
- **Output Guarantees:** Ensures output is properly null-terminated

## Performance Considerations

### Memory Efficiency
- **Buffer Reuse:** Reuses buffers where possible to minimize allocations
- **In-Place Processing:** Modifies argument arrays in place when safe
- **Early Termination:** Stops processing on first error to save resources

### Processing Optimization
- **State Caching:** Caches quote states to avoid recomputation
- **Character-by-Character:** Processes strings character by character for efficiency
- **Minimal Copying:** Reduces string copying through careful buffer management

---

*This quote processing system provides comprehensive handling of shell quoting rules while maintaining compatibility with bash-style quote semantics and variable expansion behavior.*
