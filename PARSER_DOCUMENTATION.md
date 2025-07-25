# MiniShell Parser System Documentation

This document provides detailed explanations of the parsing system in the minishell project, focusing on logical operators, parentheses handling, and command execution flow.

## Parser System Overview

The parser system in minishell handles complex command structures including:
- **Logical Operators:** `&&` (AND) and `||` (OR) with proper precedence
- **Parentheses Grouping:** Command grouping and subshell execution
- **Command Chaining:** Sequential command execution based on exit status
- **Syntax Validation:** Error detection and reporting

## Core Parser Function

### `parser_and_or(t_shell *shell, t_split split)`
**Location:** `parser/and_or_parser/parser_and_or.c`

**Purpose:** Main parser entry point that handles logical operators and parentheses grouping.

**Process Flow:**
1. **Syntax Validation:** Check for operator and parentheses syntax errors
2. **Parentheses Processing:** Handle grouped commands and subshells
3. **Operator Parsing:** Process `&&` and `||` operators with precedence
4. **Command Execution:** Execute commands based on logical conditions

**Key Features:**
- **Short-circuit Evaluation:** `&&` stops on first failure, `||` stops on first success
- **Exit Status Management:** Properly propagates exit codes through command chains
- **Error Handling:** Comprehensive syntax error detection and reporting

## Logical Operator Handling

### AND Operator (`&&`)
**Behavior:** Execute next command only if previous command succeeded (exit status 0)

**Example:**
```bash
echo "hello" && echo "world"  # Both execute if first succeeds
false && echo "world"         # Second command skipped
```

### OR Operator (`||`)
**Behavior:** Execute next command only if previous command failed (exit status != 0)

**Example:**
```bash
true || echo "world"          # Second command skipped
false || echo "world"         # Second command executes
```

## Parentheses System

### Command Grouping
**Purpose:** Groups commands for logical operator precedence

**Example:**
```bash
(echo "a" && echo "b") || echo "c"
```

### Subshell Execution
**Purpose:** Executes grouped commands in a separate shell process

**Benefits:**
- **Variable Isolation:** Changes don't affect parent shell
- **Process Isolation:** Independent signal handling
- **Exit Status:** Subshell exit status used for logical operations

## Parser Component Functions

### Core Parser Functions

#### `parser_and_or(t_shell *shell, t_split split)`
**Location:** `parser/and_or_parser/parser_and_or.c` (lines 81-96)

```c
void parser_and_or(t_shell *shell, t_split split)
{
    char *cut_indexs;

    if (split.size <= 0 || split.start == NULL)  // Guard against invalid input
        return;
    if (handle_syntax_validation(shell, split))  // Check syntax errors first
        return;
    if (handle_parentheses_processing(shell, &split))  // Process grouped commands
        return;
    if (count_str_split(split, "||", 1) + count_str_split(split, "&&", 1) > 0)
    {
        cut_indexs = get_cut_indexs(split);      // Get operator positions
        parse_and_or(shell, split, cut_indexs);  // Recursive processing
        free(cut_indexs);
    }
    else
        shell->past_exit_status = execute_command(split, shell);  // Execute single command
}
```

**Purpose:** Main entry point for parsing logical operators and command groups.

**Algorithm:**
1. **Input Validation:** Ensures split has valid content
2. **Syntax Check:** Validates operator and parentheses syntax
3. **Parentheses Processing:** Handles grouped commands first (highest precedence)
4. **Operator Processing:** Recursively processes `&&` and `||` chains
5. **Command Execution:** Executes single commands when no operators remain

#### `parse_and_or(t_shell *shell, t_split split, char *c_i)` (static)
**Location:** `parser/and_or_parser/parser_and_or.c` (lines 61-79)

```c
static void parse_and_or(t_shell *shell, t_split split, char *c_i)
{
    int vars[4];  // [start_pos, paren_count, current_pos, operator_index]

    vars[2] = -1;    // current_pos starts at -1 (incremented first)
    vars[1] = 0;     // paren_count tracks nested parentheses  
    vars[0] = 0;     // start_pos marks beginning of current segment
    vars[3] = -2;    // operator_index tracks which operator we're processing
    while (++vars[2] <= split.size)
    {
        if (vars[2] < split.size)
            vars[1] += countchr_not_quote(split.start[vars[2]], '(');  // Count opening parens
        if (vars[1] == 0 && (vars[2] == split.size 
                || is_andor(split.start[vars[2]])))  // Found operator at top level
            process_segment(shell, split, vars, c_i);
        if (vars[2] < split.size)
            vars[1] -= countchr_not_quote(split.start[vars[2]], ')');  // Count closing parens
    }
}
```

**Purpose:** Internal recursive function that processes operator segments with parentheses awareness.

**Key Logic:**
- **Parentheses Counting:** Tracks nesting depth to ignore operators inside groups
- **Segment Processing:** Only processes operators at the top level (paren_count == 0)
- **Recursive Execution:** Each segment can contain its own operator chains

### Syntax Validation Functions

#### `handle_syntax_validation(t_shell *shell, t_split split)` (static)
**Location:** `parser/and_or_parser/parser_and_or.c` (lines 16-27)

```c
static int handle_syntax_validation(t_shell *shell, t_split split)
{
    if (!check_op_errs(split)           // Validate operator syntax
        || !check_par_errs(split)       // Validate parentheses syntax
        || paranthesis_parity_check(split) == 0)  // Check balanced parentheses
    {
        if (paranthesis_parity_check(split) == 0 && !shell->in_subshell)
            write(STDERR_FILENO, "bash: syntax error: unexpected end of file\n", 43);
        shell->past_exit_status = 2;    // Set error exit status
        return (1);                     // Error found
    }
    return (0);                         // No errors
}
```

**Purpose:** Comprehensive syntax validation before parsing begins.

**Validation Steps:**
1. **Operator Errors:** Checks for malformed operator usage
2. **Parentheses Errors:** Validates parentheses syntax patterns
3. **Balance Check:** Ensures all parentheses are properly matched
4. **Error Reporting:** Sets exit status and displays appropriate error messages

#### `check_op_errs(t_split split)`
**Location:** `parser/and_or_parser/syntax.c` (lines 45-64)

```c
int check_op_errs(t_split split)
{
    int     i;
    char    *last_token;

    if (split.size > 0)
    {
        last_token = split.start[split.size - 1];
        if (ft_strncmp(last_token, "&&", 3) == 0        // Command ends with &&
            || ft_strncmp(last_token, "||", 3) == 0)    // Command ends with ||
            return (write(STDERR_FILENO, 
                    "bash: syntax error: unexpected end of file2\n", 44), 0);
    }
    i = -1;
    while (++i < split.size - 1)
    {
        if ((ft_strncmp(split.start[i], "&&", 3) == 0      // Current is operator
                || ft_strncmp(split.start[i], "||", 3) == 0)
            && (ft_strncmp(split.start[i + 1], "&&", 3) == 0   // Next is also operator
                || ft_strncmp(split.start[i + 1], "||", 3) == 0))
            return (print_syntax_error(split.start[i + 1]), 0);  // Consecutive operators
    }
    return (1);  // No errors found
}
```

**Purpose:** Validates logical operator placement and usage.

**Error Detection:**
- **Trailing Operators:** Commands cannot end with `&&` or `||`
- **Consecutive Operators:** Two operators cannot appear consecutively
- **Missing Commands:** Operators must have commands on both sides

#### `check_par_errs(t_split split)`
**Location:** `parser/and_or_parser/syntax.c` (lines 66-94)

```c
int check_par_errs(t_split split)
{
    int i;

    i = -1;
    while (++i < split.size)
    {
        if (ft_strnstr_not_q(split.start[i], "()", ft_strlen(split.start[i])))
            return (write(STDERR_FILENO, 
                    "bash: syntax error near unexpected token `)'\n", 45), 0);
        if (i < split.size - 1 && ft_strncmp(split.start[i], "(", 2) == 0
            && ft_strncmp(split.start[i + 1], ")", 2) == 0)
            return (write(STDERR_FILENO,
                    "bash: syntax error near unexpected token `)'\n", 45), 0);
        if (i < split.size - 1 && ft_strchr(split.start[i], ')')
            && ft_strchr(split.start[i + 1], '('))
            return (write(STDERR_FILENO,
                    "bash: syntax error near unexpected token `('\n", 45), 0);
        // Additional validation for proper command placement after parentheses
    }
    return (1);
}
```

**Purpose:** Validates parentheses syntax and placement rules.

**Error Detection:**
- **Empty Parentheses:** `()` patterns are invalid
- **Split Parentheses:** `(` and `)` as separate tokens are invalid
- **Improper Sequences:** `)` followed by `(` without operator is invalid
- **Missing Operators:** Commands after `)` must be separated by operators

#### `ft_strnstr_not_q(const char *haystack, const char *needle, size_t len)` (static)
**Location:** `parser/and_or_parser/syntax.c` (lines 14-36)

```c
static char *ft_strnstr_not_q(const char *haystack, const char *needle, size_t len)
{
    size_t  needle_len;
    size_t  i;
    int     inquote;

    if (!*needle)
        return ((char *)haystack);
    if (len == 0)
        return (NULL);
    needle_len = ft_strlen(needle);
    i = 0;
    inquote = 0;  // Track quote state: 0=none, 1=single, 2=double
    while (haystack[i] && i + needle_len <= len)
    {
        inquote ^= (haystack[i] == '\"' && inquote != 2);  // Toggle double quotes
        inquote ^= 2 * (haystack[i] == '\'' && inquote != 1);  // Toggle single quotes  
        if (inquote == 0 && haystack[i] == *needle          // Outside quotes
            && ft_strncmp(haystack + i, needle, needle_len) == 0)
            return ((char *)(haystack + i));
        i++;
    }
    return (NULL);
}
```

**Purpose:** String search that ignores patterns inside quoted sections.

**Quote Handling:**
- **State Tracking:** Uses bit flags to track single/double quote states
- **Mutual Exclusion:** Single quotes inside double quotes are ignored (and vice versa)
- **Pattern Matching:** Only matches needle when outside any quotes

#### `print_syntax_error(char *token)` (static)
**Location:** `parser/and_or_parser/syntax.c` (lines 38-43)

```c
static void print_syntax_error(char *token)
{
    write(STDERR_FILENO, "bash: syntax error near unexpected token `", 42);
    write(STDERR_FILENO, token, ft_strlen(token));  // Print the problematic token
    write(STDERR_FILENO, "'\n", 2);
}
```

**Purpose:** Standardized error message formatting for syntax errors.

#### `paranthesis_parity_check(t_split split)`
**Location:** `parser/and_or_parser/parser_and_or_utils.c` (lines 79-89)

```c
int paranthesis_parity_check(t_split split)
{
    int i;
    int par;

    i = -1;
    par = 0;  // Running count of parentheses balance
    while (++i < split.size)
        par += count_parentheses_in_string(split.start[i]);  // Sum all parentheses
    if (par == 0)       // Balanced: equal opening and closing
        return (1);     // Valid
    return (0);         // Unbalanced
}
```

**Purpose:** Ensures parentheses are properly balanced across the entire command.

**Balance Logic:**
- **Counting:** Each `(` adds 1, each `)` subtracts 1
- **Quote Awareness:** Only counts parentheses outside quoted sections
- **Zero Sum:** Final count must be zero for proper balance

#### `count_parentheses_in_string(char *str)` (static)
**Location:** `parser/and_or_parser/parser_and_or_utils.c` (lines 57-77)

```c
static int count_parentheses_in_string(char *str)
{
    int j;
    int par;
    int quote_state;

    j = -1;
    par = 0;
    quote_state = 0;  // Bit flags: 1=single quote, 2=double quote
    while (str[++j] != '\0')
    {
        if (str[j] == '\'' && !(quote_state & 2))      // Single quote outside double
            quote_state ^= 1;
        else if (str[j] == '"' && !(quote_state & 1))  // Double quote outside single
            quote_state ^= 2;
        else if (!quote_state)                         // Outside any quotes
            par += (str[j] == '(') - (str[j] == ')');  // Count parentheses
    }
    return (par);
}
```

**Purpose:** Counts parentheses balance in a single string, ignoring quoted sections.

**Quote State Management:**
- **Bit Masking:** Uses bit operations for efficient quote state tracking
- **Nested Immunity:** Quotes inside other quotes are treated as literal characters
- **Arithmetic Counting:** Efficient parentheses counting with single expression

### Parentheses Processing Functions

#### `handle_parentheses_processing(t_shell *shell, t_split *split)` (static)
**Location:** `parser/and_or_parser/parser_and_or.c` (lines 29-42)

```c
static int handle_parentheses_processing(t_shell *shell, t_split *split)
{
    int was_in_subshell;

    if (check_single_par(*split) != 0)  // Check if entire command is wrapped in parentheses
    {
        was_in_subshell = shell->in_subshell;    // Save current subshell state
        shell->in_subshell = 1;                  // Mark as subshell execution
        cut_out_par(split);                      // Remove outer parentheses
        parser_and_or(shell, *split);            // Recursively parse inner content
        shell->in_subshell = was_in_subshell;   // Restore previous state
        return (1);                              // Processed parentheses
    }
    return (0);                                  // No parentheses to process
}
```

**Purpose:** Handles commands wrapped in parentheses by creating subshell context.

**Subshell Management:**
- **State Preservation:** Saves and restores subshell state for nested commands
- **Recursive Processing:** Inner content is parsed as a complete command
- **Error Context:** Subshell flag affects error message display

#### `check_single_par(t_split split)`
**Location:** `parser/and_or_parser/and_or_utils.c` (lines 81-91)

```c
int check_single_par(t_split split)
{
    char *start;
    char *end;

    start = split.start[0];                           // First token
    end = split.start[split.size - 1];               // Last token
    if (start[0] == '(' && end[ft_strlen(end) - 1] == ')'   // Starts with ( and ends with )
        && check_symbol(split, "||", 1) < 0              // No || operators at top level
        && check_symbol(split, "&&", 1) < 0)             // No && operators at top level  
        return (1);                                    // Single parenthetical group
    return (0);                                        // Not a single group
}
```

**Purpose:** Determines if the entire command is wrapped in a single set of parentheses.

**Validation Logic:**
- **Boundary Check:** First character is `(`, last character is `)`
- **Operator Exclusion:** No top-level logical operators (which would break the grouping)
- **Integrity Check:** Ensures the parentheses actually contain a single logical unit

#### `check_symbol(t_split split, char *find, int flag)` (static)
**Location:** `parser/and_or_parser/and_or_utils.c` (lines 13-29)

```c
static int check_symbol(t_split split, char *find, int flag)
{
    int count;
    int i;

    i = 0;
    count = 0;  // Parentheses nesting level
    while (i < split.size)
    {
        count = count + countchr_not_quote(split.start[i], '(');  // Add opening parens
        if (flag == 0 || count == 0)  // At top level (when flag=1) or always (when flag=0)
        {
            if (ft_strncmp(find, split.start[i], ft_strlen(find)) == 0)
                return (i);  // Found operator at specified level
        }
        count = count - countchr_not_quote(split.start[i], ')');  // Subtract closing parens
        i++;
    }
    return (-2);  // Not found
}
```

**Purpose:** Searches for a specific operator at the specified nesting level.

**Parameters:**
- **find:** The operator string to search for (e.g., "&&", "||")
- **flag:** When 1, only finds operators at top level (count == 0)

#### `cut_out_par(t_split *split)`
**Location:** `parser/and_or_parser/parser_and_or.c` (lines 44-60)

```c
void cut_out_par(t_split *split)
{
    char *start;
    char *end;

    start = split->start[0];                     // First token
    end = split->start[split->size - 1];        // Last token
    if (is_singlepar(start, end))               // Special case: token contains only parentheses
    {
        split->size--;                          // Simply reduce size
        return;
    }
    remove_outer_par(start, end);               // Remove outer parentheses from tokens
    clean_empties(split);                       // Clean up any empty tokens created
}
```

**Purpose:** Removes outer parentheses from a command group.

**Process:**
1. **Special Case Detection:** Handles tokens that are purely parentheses
2. **Character Removal:** Strips outer parentheses from first and last tokens  
3. **Cleanup:** Removes any empty tokens created by the process

#### `is_singlepar(char *start, char *end)` (static)
**Location:** `parser/and_or_parser/parser_and_or_parentheses_utils.c` (lines 15-21)

```c
int is_singlepar(char *start, char *end)
{
    if (start == end && (size_t)(countchr_str(start, '(')     // Same token containing
        + countchr_str(start, ')')) == ft_strlen(start))      // only parentheses
        return (1);
    return (0);
}
```

**Purpose:** Checks if a token contains only parentheses characters.

**Logic:** Token length equals the sum of all parentheses characters it contains.

#### `remove_outer_par(char *start, char *end)` (static)
**Location:** `parser/and_or_parser/parser_and_or_parentheses_utils.c` (lines 23-33)

```c
void remove_outer_par(char *start, char *end)
{
    int start_len;

    if (start[0] == '(' && end[ft_strlen(end) - 1] == ')')  // Valid outer parentheses
    {
        end[ft_strlen(end) - 1] = '\0';          // Remove closing paren from end token
        start_len = ft_strlen(start);
        ft_memmove(&(start[0]), &(start[1]), start_len - 1);  // Shift start token left
        start[start_len - 1] = '\0';             // Null terminate
    }
}
```

**Purpose:** Physically removes the outer parentheses characters from boundary tokens.

**Memory Management:**
- **In-place Modification:** Modifies strings directly to avoid allocation
- **Safe Movement:** Uses `ft_memmove` for overlapping memory regions
- **Null Termination:** Properly terminates modified strings

#### `clean_empties(t_split *split)` (static)
**Location:** `parser/and_or_parser/parser_and_or_parentheses_utils.c` (lines 35-50)

```c
void clean_empties(t_split *split)
{
    char *start;
    char *end;

    start = split->start[0];                    // First token
    end = split->start[split->size - 1];       // Last token
    if (split->size > 1 && start[0] == '\0')   // First token is empty and not the only token
    {
        split->start = &(split->start[1]);     // Advance start pointer
        split->size--;                         // Reduce size
    }
    if (end[0] == '\0')                        // Last token is empty
        split->size--;                         // Reduce size
}
```

**Purpose:** Removes empty tokens created by parentheses removal.

**Safety:** Only removes empty first token when multiple tokens exist to prevent invalid splits.

### Operator Processing Functions

#### `count_str_split(t_split split, const char *str, int flag)`
**Location:** `parser/and_or_parser/parser_and_or2.c` (lines 16-32)

```c
int count_str_split(t_split split, const char *str, int flag)
{
    int i;
    int count;
    int par;

    i = 0;
    count = 0;
    par = 0;  // Parentheses nesting level
    while (i < split.size)
    {
        par += countchr_not_quote(split.start[i], '(');      // Track opening parens
        if ((flag == 0 || par == 0)                         // At specified level
            && (ft_strncmp(str, split.start[i], 4) == 0))   // Found matching operator
            count++;
        par -= countchr_not_quote(split.start[i], ')');     // Track closing parens
        i++;
    }
    return (count);
}
```

**Purpose:** Counts occurrences of a specific operator at the specified nesting level.

**Parameters:**
- **str:** Operator to count (e.g., "&&", "||")
- **flag:** When 1, only counts operators at top level (par == 0)

**Usage:** Used to determine if operator processing is needed and for memory allocation.

## Command Execution and Validation Functions

### `get_cut_indexs(t_split split)`
**Location:** `parser/and_or_parser/parser_and_or_utils.c` (lines 35-55)

```c
char *get_cut_indexs(t_split split)
{
    int     i;
    int     par;                    // Parentheses nesting level
    char    *ret;
    int     check;                  // Current operator index
    int     result;

    i = -1;
    par = 0;
    check = 0;
    ret = allocate_cut_indexes(split);              // Allocate result array
    if (ret == NULL)
        return (NULL);
    while (++i < split.size)
    {
        par += countchr_not_quote(split.start[i], '(');    // Track opening parentheses
        result = process_cut_index(split, i, par);         // Check for operators at this level
        if (result != -1)
            ret[check++] = '0' + result;                   // Store operator type (0='||', 1='&&')
        par -= countchr_not_quote(split.start[i], ')');    // Track closing parentheses
    }
    return (ret);
}
```

**Purpose:** Creates an index array that maps logical operators to their execution precedence.

**Algorithm:**
1. **Nesting Tracking:** Monitors parentheses depth to find top-level operators
2. **Operator Detection:** Identifies `&&` and `||` operators at the current level
3. **Index Creation:** Builds array where each position corresponds to an operator
4. **Type Encoding:** '0' represents `||`, '1' represents `&&`

**Return Value:** String where each character encodes an operator type for conditional execution

### `process_cut_index(t_split split, int i, int par)` (static)
**Location:** `parser/and_or_parser/parser_and_or_utils.c` (lines 20-32)

```c
static int process_cut_index(t_split split, int i, int par)
{
    if (par == 0)                                          // Only process top-level operators
    {
        if (ft_strncmp("||", split.start[i], 3) == 0)      // Found OR operator
            return (1);
        else if (ft_strncmp("&&", split.start[i], 3) == 0) // Found AND operator
            return (0);
    }
    return (-1);                                           // No operator found
}
```

**Purpose:** Checks if current token is a logical operator at the specified nesting level.

**Level Filtering:** Only processes operators at top level (par == 0) to respect parentheses grouping

**Return Codes:**
- **0:** AND operator (`&&`)
- **1:** OR operator (`||`)
- **-1:** Not an operator or nested inside parentheses

### `execute_command(t_split split, t_shell *shell)`
**Location:** `execute/exec_dispatch.c` (lines 46-67)

```c
int execute_command(t_split split, t_shell *shell)
{
    int exit_status;
    int has_pipes;                      // Number of pipe operators
    int has_redir;                      // Boolean: has redirections
    int syntax_error;

    if (split.size == 0)                // Empty command
        return (0);
    syntax_error = validate_redirection_syntax(split);     // Check syntax first
    if (syntax_error != 0)
    {
        shell->past_exit_status = syntax_error;
        return (syntax_error);
    }
    has_pipes = count_pipes(split);                        // Count pipe operators
    has_redir = has_redirs(split);                         // Check for redirections
    exit_status = dispatch_execution(split, shell, has_pipes, has_redir);
    shell->past_exit_status = exit_status;                 // Update shell state
    return (exit_status);
}
```

**Purpose:** Main command execution entry point that validates syntax and dispatches to appropriate execution method.

**Execution Flow:**
1. **Empty Check:** Returns immediately for empty commands
2. **Syntax Validation:** Ensures redirections and pipes are syntactically correct
3. **Feature Detection:** Counts pipes and checks for redirections
4. **Execution Dispatch:** Routes to specialized execution functions
5. **State Update:** Updates shell's past exit status

**Integration:** Central hub connecting parser output to execution subsystems

### `validate_redirection_syntax(t_split split)`
**Location:** `execute/redirections/redir_syntax.c` (lines 82-101)

```c
int validate_redirection_syntax(t_split split)
{
    int i;
    int has_parentheses;
    int err;
    int redirect_type;

    i = -1;
    has_parentheses = has_parentheses_in_split(split);     // Check for parentheses
    while (++i < split.size)
    {
        redirect_type = is_redirection(split.start[i]);    // Check if token is redirection
        if (redirect_type)
        {
            err = handle_redirection_token(split, &i);     // Validate redirection
            if (err)
                return (err);
        }
        err = handle_pipe_token(split, i, has_parentheses); // Validate pipe placement
        if (err)
            return (err);
    }
    return (0);                                            // No syntax errors
}
```

**Purpose:** Comprehensive syntax validation for redirection operators and pipes.

**Validation Checks:**
- **Redirection Completeness:** Each redirection has a filename
- **Pipe Placement:** Pipes not at beginning/end (unless parentheses present)
- **Token Sequencing:** No consecutive redirections or invalid combinations

**Error Handling:** Returns specific error codes for different syntax violations

### `has_parentheses_in_split(t_split split)`
**Location:** `execute/redirections/redir_syntax.c` (lines 16-26)

```c
int has_parentheses_in_split(t_split split)
{
    int i;

    i = 0;
    while (i < split.size)
    {
        if (ft_strchr(split.start[i], '(') || ft_strchr(split.start[i], ')'))  // Found parenthesis
            return (1);                                    // Parentheses present
        i++;
    }
    return (0);                                            // No parentheses found
}
```

**Purpose:** Detects presence of parentheses in command tokens for context-aware validation.

**Usage:** Parentheses affect pipe validation rules (pipes at boundaries are valid in subshells)

### `is_redirection(char *token)`
**Location:** `execute/redirections/redir_utils.c` (lines 13-25)

```c
int is_redirection(char *token)
{
    if (!token)
        return (0);
    if (ft_strncmp(token, "<<", 3) == 0 && ft_strlen(token) == 2)  // Here document
        return (1);
    if (ft_strncmp(token, ">>", 3) == 0 && ft_strlen(token) == 2)  // Append output
        return (2);
    if (ft_strncmp(token, "<", 2) == 0 && ft_strlen(token) == 1)   // Input redirect
        return (3);
    if (ft_strncmp(token, ">", 2) == 0 && ft_strlen(token) == 1)   // Output redirect
        return (4);
    return (0);                                                    // Not a redirection
}
```

**Purpose:** Identifies and categorizes redirection operators.

**Return Codes:**
- **0:** Not a redirection operator
- **1:** Here document (`<<`)
- **2:** Append output (`>>`)
- **3:** Input redirection (`<`)
- **4:** Output redirection (`>`)

**Strict Matching:** Ensures exact token match to avoid false positives

### `handle_redirection_token(t_split split, int *i)` (static)
**Location:** `execute/redirections/redir_syntax.c` (lines 58-66)

```c
static int handle_redirection_token(t_split split, int *i)
{
    int err;

    err = check_redirection_error(split, *i);              // Validate this redirection
    if (err)
        return (err);
    *i += 1;                                               // Skip filename token
    return (0);
}
```

**Purpose:** Validates individual redirection tokens and advances iterator.

**Side Effect:** Increments iterator to skip over the filename token that follows

### `check_redirection_error(t_split split, int i)`
**Location:** `execute/redirections/redir_syntax_utils.c` (lines 35-53)

```c
int check_redirection_error(t_split split, int i)
{
    int     filename_index;
    char    *next_token;

    filename_index = i + 1;                                // Expected filename position
    if (filename_index >= split.size)                      // No filename provided
    {
        write(STDERR_FILENO,
            "bash: syntax error near unexpected token `newline'\n", 52);
        return (2);
    }
    next_token = split.start[filename_index];
    if (is_redirection(next_token) || ft_strncmp(next_token, "|", 2) == 0)  // Invalid filename
    {
        print_token_error(next_token);                     // Report specific error
        return (2);
    }
    return (0);                                            // Valid redirection
}
```

**Purpose:** Validates that redirection operators have proper filenames.

**Error Conditions:**
- **Missing Filename:** Redirection at end of command
- **Invalid Filename:** Next token is another operator or pipe

**Error Messages:** Provides bash-compatible error messages for syntax violations

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

**Purpose:** Counts the number of pipe operators in the command for execution planning.

**Usage:** Determines whether command needs pipeline execution or single command execution

### `has_redirs(t_split split)`
**Location:** `execute/arg_utils.c` (lines 13-23)

```c
int has_redirs(t_split split)
{
    int i;

    i = 0;
    while (i < split.size)
    {
        if (is_redirection(split.start[i]))                // Found any redirection
            return (1);
        i++;
    }
    return (0);                                            // No redirections found
}
```

**Purpose:** Determines if command contains any redirection operators.

**Boolean Return:** 1 if redirections present, 0 otherwise

### `dispatch_execution(t_split split, t_shell *shell, int has_pipes, int has_redir)` (static)
**Location:** `execute/exec_dispatch.c` (lines 35-45)

```c
static int dispatch_execution(t_split split, t_shell *shell,
        int has_pipes, int has_redir)
{
    if (has_pipes > 0 && has_redir)                        // Pipes AND redirections
        return (execute_pipe_redir(split, shell));
    else if (has_pipes > 0)                                // Pipes only
        return (execute_pipeline(split, shell));
    else if (has_redir)                                    // Redirections only
        return (execute_with_redirections(split, shell));
    else                                                   // Simple command
        return (execute_args_array(split, shell));
}
```

**Purpose:** Routes commands to appropriate execution function based on features present.

**Execution Paths:**
- **Complex:** Pipes + redirections → `execute_pipe_redir()` (documented in [EXECUTE_PIPE_REDIR_DOCUMENTATION.md](EXECUTE_PIPE_REDIR_DOCUMENTATION.md))
- **Pipeline:** Pipes only → `execute_pipeline()` (documented in [EXECUTE_PIPELINE_DOCUMENTATION.md](EXECUTE_PIPELINE_DOCUMENTATION.md))
- **Redirected:** Redirections only → `execute_with_redirections()` (documented in [EXECUTE_WITH_REDIRECTIONS_DOCUMENTATION.md](EXECUTE_WITH_REDIRECTIONS_DOCUMENTATION.md))
- **Simple:** Neither → `execute_args_array()` (documented in [EXECUTE_ARGS_ARRAY_DOCUMENTATION.md](EXECUTE_ARGS_ARRAY_DOCUMENTATION.md))

**Design Pattern:** Strategy pattern that selects execution strategy based on command characteristics

### Error Handling
- **Syntax Errors:** Reports specific syntax error messages
- **Exit Status:** Sets appropriate exit codes for different error types
- **Recovery:** Maintains shell stability after errors

## Integration with Main Shell

The parser system integrates seamlessly with the main shell execution flow:

1. **Input Processing:** Receives tokenized input from main command processing
2. **Parse Tree Building:** Constructs logical command structure
3. **Execution Control:** Manages command execution based on logical conditions
4. **Status Propagation:** Returns appropriate exit status to main shell

## Future Extensions

The parser system is designed to support future enhancements:
- **Additional Operators:** Easy addition of new logical operators
- **Complex Grouping:** Support for nested command structures
- **Optimization:** Performance improvements for large command chains

---

*For implementation details of specific parser functions, refer to the source files in `parser/and_or_parser/`.*
