# MiniShell Core Functions and Structures Documentation

This document provides detailed explanations of the core structures and functions in the minishell project, their purposes, interactions, and specific implementation details.

## Core Data Structure

### `t_shell` struct
**Location:** `main/minishell.h` (lines 41-51)

```c
typedef struct s_shell
{
    int     past_exit_status;    // Last command exit code
    char    *current_input;      // Current command line input
    t_split split_input;         // Tokenized input structure
    char    **envp;              // Environment variables array
    int     should_exit;         // Exit flag for main loop
    int     exit_code;           // Final exit code for shell
    int     in_subshell;         // Subshell execution flag
    char    *prompt;             // Current prompt string
    char    *terminal_prompt;    // Static prompt template
} t_shell;
```

**Purpose:** Central data structure containing all shell state information.

**Key Fields:**
- `past_exit_status`: Stores the exit code from the last executed command, used for `$?` variable expansion
- `current_input`: Holds the raw command line input from readline
- `split_input`: Parsed and tokenized version of current_input using t_split structure
- `envp`: Dynamic copy of environment variables, modifiable by export/unset commands
- `should_exit`: Boolean flag set by exit command to terminate main loop
- `exit_code`: Final exit status when shell terminates
- `in_subshell`: Flag indicating if currently executing in a subshell context
- `prompt`: Dynamic prompt string (currently points to terminal_prompt)
- `terminal_prompt`: Static string "minishell > " used as the shell prompt

---

## Initialization Functions

### `shell_init(t_shell *shell)`
**Location:** `main/main_utils.c` (lines 46-56)

```c
void shell_init(t_shell *shell)
{
    shell->current_input = NULL;
    shell->split_input = create_split(NULL, 0);
    shell->past_exit_status = 0;
    shell->should_exit = 0;
    shell->exit_code = 0;
    shell->in_subshell = 0;
    shell->envp = NULL;
    shell->prompt = NULL;
    shell->terminal_prompt = "minishell > ";
}
```

**Purpose:** Initializes all shell structure fields to safe default values.

**Specific Values Set:**
- `current_input`: NULL (no input yet)
- `split_input`: Empty t_split structure created by `create_split(NULL, 0)`
- `past_exit_status`: 0 (successful start)
- `should_exit`: 0 (false - don't exit)
- `exit_code`: 0 (default success code)
- `in_subshell`: 0 (false - main shell)
- `envp`: NULL (will be set by `init_env`)
- `prompt`: NULL (currently unused)
- `terminal_prompt`: "minishell > " (constant prompt string)

### `init_env(t_shell *shell, char **envp)`
**Location:** `main/environment.c` (lines 14-30)

```c
void init_env(t_shell *shell, char **envp)
{
    int i;
    int count;

    count = 0;
    while (envp[count])
        count++;
    shell->envp = malloc(sizeof(char *) * (count + 1));
    if (!shell->envp)
        return ;
    i = 0;
    while (i < count)
    {
        shell->envp[i] = ft_strdup(envp[i]);
        if (!shell->envp[i])
        {
            // Error handling: free previously allocated strings
            while (--i >= 0)
                free(shell->envp[i]);
            free(shell->envp);
            shell->envp = NULL;
            return ;
        }
        i++;
    }
    shell->envp[i] = NULL;
}
```

**Purpose:** Creates a dynamic copy of the system environment variables.

**Why Dynamic Copy:**
- Allows modification via export/unset commands
- Original system envp is read-only
- Memory is managed by shell for proper cleanup
- Each string is duplicated using `ft_strdup` for independence

**Error Handling:**
- If any allocation fails, all previously allocated strings are freed
- Shell continues with NULL envp rather than crashing

---

## Main Execution Loop

### `start_shell(t_shell *shell)`
**Location:** `main/main.c` (lines 42-65)

```c
static void start_shell(t_shell *shell)
{
    setup_signals();
    while (!shell->should_exit)
    {
        g_signal = 0;
        shell->prompt = readline(shell->terminal_prompt);
        shell->current_input = shell->prompt;
        if (shell->current_input == NULL)
            safe_exit(shell);
        if (g_signal == SIGINT)
        {
            shell->past_exit_status = 130;
            g_signal = 0;
        }
        if (!is_empty(shell->current_input))
        {
            add_history(shell->current_input);
            begin_command_parsing_and_execution(shell);
        }
        free(shell->current_input);
    }
    free_environment(shell);
    rl_clear_history();
    exit(shell->exit_code);
}
```

**Purpose:** Main shell loop handling user input and command execution.

**Key Operations:**
1. **Signal Setup:** Calls `setup_signals()` once before loop
2. **Input Reading:** Uses GNU Readline `readline()` with terminal_prompt
3. **EOF Handling:** NULL return from readline triggers `safe_exit()`
4. **Signal Handling:** SIGINT sets exit status to 130 (128 + SIGINT)
5. **History Management:** Non-empty inputs added to readline history
6. **Command Processing:** Calls parsing and execution pipeline
7. **Cleanup:** Frees environment and readline history before exit

**Signal Integration:**
- `g_signal` checked after readline to handle interrupts
- Exit status 130 matches bash behavior for SIGINT

### `setup_signals(void)`
**Location:** `execute/signal.c` (lines 42-56)

```c
void setup_signals(void)
{
    struct sigaction sa_int;
    struct sigaction sa_quit;

    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_handler = signal_handler;
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);
    
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_handler = SIG_IGN;
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);
}
```

**Purpose:** Configures signal handling for interactive shell behavior.

**Signal Configuration:**
- **SIGINT (Ctrl+C):** Custom handler with SA_RESTART flag
- **SIGQUIT (Ctrl+\\):** Ignored (SIG_IGN) in interactive mode

**Handler Behavior (signal_handler):**
- Sets global `g_signal = SIGINT`
- Writes newline to stdout
- Calls `rl_on_new_line()` and `rl_replace_line("", 0)`
- Calls `rl_redisplay()` to refresh prompt

**SA_RESTART Flag:** Automatically restarts interrupted system calls (like readline)

### `readline(const char *prompt)` 
**Library Function:** GNU Readline Library

**Purpose:** Reads a line from the terminal with editing capabilities.

**Features Provided:**
- Line editing (arrow keys, backspace, etc.)
- Command history (up/down arrows)
- Tab completion (if configured)
- Signal-safe input reading

**Return Values:**
- **Non-NULL:** Pointer to allocated string containing input line
- **NULL:** EOF condition (Ctrl+D) or error

**Memory Management:** Caller must free returned string

### `safe_exit(t_shell *shell)`
**Location:** `main/main_utils.c` (lines 26-40)

```c
void safe_exit(t_shell *shell)
{
    if (shell->current_input != NULL)
        free(shell->current_input);
    if (shell->split_input.start != NULL)
        free_split(&(shell->split_input));
    free_env(shell);
    rl_clear_history();
    printf("exit\n");
    exit(shell->past_exit_status);
}
```

**Purpose:** Handles graceful shell termination with proper cleanup.

**Cleanup Operations:**
1. **Input Memory:** Frees current_input if allocated
2. **Split Structure:** Calls `free_split()` to free tokenized input
3. **Environment:** Calls `free_env()` to free envp array
4. **Readline History:** Calls `rl_clear_history()` to free history
5. **User Notification:** Prints "exit" to match bash behavior
6. **Process Exit:** Exits with `past_exit_status` code

**When Called:**
- EOF detected from readline (Ctrl+D)
- Explicit exit command (through should_exit flag)

### `g_signal` Global Variable
**Location:** `main/main.c` (line 16)

```c
extern volatile sig_atomic_t g_signal;
```

**Purpose:** Thread-safe signal communication between signal handlers and main code.

**Type Significance:**
- `volatile`: Prevents compiler optimization, ensures variable is read from memory
- `sig_atomic_t`: Guaranteed atomic read/write operations, signal-safe

**Usage Pattern:**
1. Reset to 0 at start of each loop iteration
2. Set by signal handlers (signal_handler, signal_handler_child)
3. Checked after potentially blocking operations (readline)
4. Values: 0 (no signal), SIGINT (2), SIGQUIT (3)

**Signal Safety:** Only variable type guaranteed safe for signal handler access

### `is_empty(char *str)`
**Location:** `main/main_utils.c` (lines 14-24)

```c
int is_empty(char *str)
{
    int i;

    if (str == NULL)
        return (1);
    i = 0;
    while (str[i] != '\0')
    {
        if (!(str[i] == ' ' || (str[i] <= 13 && str[i] >= 9)))
            return (0);
        i++;
    }
    return (1);
}
```

**Purpose:** Determines if string contains only whitespace characters.

**Whitespace Definition:**
- Space character (' ')
- ASCII characters 9-13: tab, newline, vertical tab, form feed, carriage return

**Return Values:**
- **1 (true):** String is NULL or contains only whitespace
- **0 (false):** String contains at least one non-whitespace character

**Usage:** Prevents empty commands from being added to history or processed

### `add_history(const char *line)`
**Library Function:** GNU Readline Library

**Purpose:** Adds command line to readline's history list.

**Features:**
- Maintains command history across readline calls
- Accessible via up/down arrow keys
- Persistent until `rl_clear_history()` called
- Automatic duplicate detection (configurable)

**Memory Management:** Readline manages history memory internally

### `begin_command_parsing_and_execution(t_shell *shell)`
**Location:** `main/main.c` (lines 18-40)

```c
static void begin_command_parsing_and_execution(t_shell *shell)
{
    char *expanded_input;

    if (!check_quotes(shell->current_input))
        shell->past_exit_status = 2;
    else
    {
        sep_opt_arg(shell);
        expanded_input = wildcard_input_modify(shell->current_input);
        if (expanded_input && expanded_input != shell->current_input)
        {
            free(shell->current_input);
            shell->current_input = expanded_input;
        }
        else if (expanded_input && expanded_input != shell->current_input)
        {
            free(shell->current_input);
            shell->current_input = expanded_input;
        }
        shell->split_input = create_split_str(shell->current_input);
        if (shell->split_input.size > 0)
            parser_and_or(shell, shell->split_input);
        free_split(&(shell->split_input));
    }
}
```

**Purpose:** Orchestrates the complete command processing pipeline.

**Processing Pipeline:**

1. **Quote Validation:** `check_quotes()` - validates balanced quotes
2. **Operator Separation:** `sep_opt_arg()` - adds spaces around operators for tokenization
3. **Wildcard Expansion:** `wildcard_input_modify()` - expands glob patterns to filenames
4. **Tokenization:** `create_split_str()` - splits input into token array
5. **Parsing & Execution:** `parser_and_or()` - processes logical operators and executes
6. **Cleanup:** `free_split()` - releases memory

**Error Handling:**
- Quote errors: Sets exit status 2, skips execution
- Memory allocation: Proper cleanup of intermediate results
- Empty input: Handled by size check before parsing

**Memory Management Notes:**
- Wildcard expansion may replace current_input
- Original input freed if replacement occurs
- Split structure always freed after processing
- Robust handling of allocation failures

---

## Input Processing and Parsing Functions

### `check_quotes(char *str)`
**Location:** `parser/quotes.c` (lines 14-24)

```c
int check_quotes(char *str)
{
    int i;
    int single_open;  // Track single quote state
    int double_open;  // Track double quote state

    if (!str)
        return (1);      // NULL input is considered valid
    i = -1;
    single_open = 0;
    double_open = 0;
    while (str[++i])
    {
        if (str[i] == '\'' && !double_open)  // Single quote outside double quotes
            single_open = !single_open;
        else if (str[i] == '"' && !single_open)  // Double quote outside single quotes
            double_open = !double_open;
    }
    return (single_open == 0 && double_open == 0);  // Both must be closed
}
```

**Purpose:** Validates that all quotes in the input string are properly balanced.

**Quote State Logic:**
- **Single quotes:** Toggle state only when not inside double quotes
- **Double quotes:** Toggle state only when not inside single quotes  
- **Nested behavior:** Single quotes inside double quotes are literal (and vice versa)

**Return Values:**
- **1 (true):** All quotes are properly balanced
- **0 (false):** Unmatched quotes found - syntax error

**Usage:** Called before parsing to prevent processing of malformed input

### `sep_opt_arg(t_shell *shell)`
**Location:** `parser/sep_opt_arg/sep_opt_arg.c` (lines 96-103)

```c
void sep_opt_arg(t_shell *shell)
{
    char *temp;
    int  sep_size;

    temp = shell->current_input;
    sep_size = get_sep_size(temp);                    // Calculate needed space
    if ((size_t)sep_size == ft_strlen(shell->current_input) + 1)
        return ;                                      // No operators found
    shell->current_input = (char *)ft_calloc(sep_size, sizeof(char));
    set_sep(shell->current_input, temp, sep_size);   // Add spaces around operators
    free(temp);                                      // Free original input
}
```

**Purpose:** Adds spaces around shell operators to facilitate proper tokenization.

**Operators Handled:** `||`, `&&`, `<<`, `>>`, `|`, `>`, `<` (in priority order)

**Why Necessary:** 
- Input like `echo hello>file` becomes `echo hello > file`
- Ensures operators are separate tokens during splitting
- Preserves quote context - operators inside quotes are not separated

### `get_sep_size(char *un_sep)` (static)
**Location:** `parser/sep_opt_arg/sep_opt_arg.c` (lines 37-58)

```c
static int get_sep_size(char *un_sep)
{
    int i;
    int extra;      // Additional space needed for operator separation
    int j;

    i = 0;
    extra = 0;
    while (un_sep[i] != '\0')
    {
        j = 0;
        while (get_inquote(un_sep, i) == 0 && j < 7)  // Check if outside quotes
        {
            if (ft_strncmp(&(un_sep[i]), get_opt(j), 1 + (j < 4)) == 0)
            {
                // Add space before if needed + space after if needed
                extra += (i > 0 && un_sep[i - 1] != ' ')
                    + (un_sep[i + 1 + (j < 4)] != '\0'
                        && un_sep[i + 1 + (j < 4)] != ' ');
                break ;
            }
            j++;
        }
        i += 1 + (j < 4 && get_inquote(un_sep, i) == 0);  // Skip operator chars
    }
    return (i + extra + 1);  // Original length + extra spaces + null terminator
}
```

**Purpose:** Calculates the total space needed after adding spaces around operators.

**Logic:**
- **Operator Detection:** Checks each position for the 7 shell operators
- **Quote Awareness:** Only processes operators outside quoted strings
- **Space Calculation:** Adds space before/after operators if not already present
- **Multi-char Operators:** Handles 2-character operators (`||`, `&&`, `<<`, `>>`)

### `set_sep(char *sep, char *un_sep, int sep_size)` (static)
**Location:** `parser/sep_opt_arg/sep_opt_arg.c` (lines 64-89)

```c
static void set_sep(char *sep, char *un_sep, int sep_size)
{
    int vars[3];  // [current_pos, extra_chars_added, operator_index]

    vars[0] = 0;  // Position in original string
    vars[1] = 0;  // Extra characters added
    while (un_sep[vars[0]] != '\0')
    {
        vars[2] = -1;  // Reset operator index
        while (get_inquote(un_sep, vars[0]) == 0 && ++vars[2] < 7)
        {
            if (ft_strncmp(&(un_sep[vars[0]]), get_opt(vars[2]), 
                    1 + (vars[2] < 4)) == 0)
            {
                // Add space before operator if needed
                if (vars[0] > 0 && un_sep[vars[0] - 1] != ' ')
                    sep[vars[0] + vars[1]++] = ' ';
                
                // Copy the operator itself
                ft_strlcat(sep, get_opt(vars[2]), sep_size);
                vars[1] += ft_strlen(get_opt(vars[2])) - 1;
                
                // Add space after operator if needed
                if (un_sep[vars[0] + 1 + (vars[2] < 4)] != '\0'
                    && un_sep[vars[0] + 1 + (vars[2] < 4)] != ' ')
                    sep[vars[0] + 1 + (vars[2] < 4) + vars[1]++] = ' ';
                break ;
            }
        }
        // Copy regular character if not an operator or inside quotes
        if (vars[2] > 6 || get_inquote(un_sep, vars[0]) != 0)
            sep[vars[0] + vars[1]] = un_sep[vars[0]];
        vars[0] += 1 + (vars[2] < 4 && get_inquote(un_sep, vars[0]) == 0);
    }
}
```

**Purpose:** Performs the actual separation by inserting spaces around operators.

**Algorithm:**
1. **Scan Original:** Iterate through each character
2. **Operator Check:** Test against all 7 operators if outside quotes
3. **Space Insertion:** Add spaces before/after operators when needed
4. **Operator Copy:** Insert the complete operator string
5. **Regular Copy:** Copy non-operator characters as-is

### `wildcard_input_modify(char *current_input)`
**Location:** `parser/wildcard_handle/wildcard_handle.c` (lines 88-110)

```c
char *wildcard_input_modify(char *current_input)
{
    t_split modified_input;
    int     i;
    char    *temp;

    modified_input = create_split_str(current_input);  // Tokenize input
    i = 0;
    while (i < modified_input.size)
    {
        // Check if token contains '*' outside quotes
        if (countchr_not_quote(modified_input.start[i], '*') > 0)
        {
            temp = wildcard_handle(modified_input.start[i]);  // Expand pattern
            if (temp != NULL && temp[0] != '\0')             // Valid expansion
            {
                free(modified_input.start[i]);               // Replace token
                modified_input.start[i] = temp;
            }
            else if (temp != NULL)                           // Empty expansion
                free(temp);                                  // Keep original
        }
        i++;
    }
    temp = revert_split_str(modified_input);                 // Reconstruct string
    free_split(&modified_input);
    return (temp);
}
```

**Purpose:** Expands wildcard patterns (*) to matching filenames in the current directory.

**Process Flow:**
1. **Tokenization:** Split input into individual tokens
2. **Pattern Detection:** Find tokens containing '*' outside quotes
3. **Expansion:** Generate space-separated list of matching files
4. **Replacement:** Replace pattern with expansion or keep original
5. **Reconstruction:** Join tokens back into a single string

**Wildcard Behavior:**
- **Quote Protection:** '*' inside quotes are literal characters
- **No Matches:** Pattern remains unchanged (bash-compatible)
- **Multiple Matches:** Files joined with spaces and single quotes

### `create_split_str(char *str)`
**Location:** `parser/t_split_utils/t_split_utils.c` (lines 38-51)

```c
t_split create_split_str(char *str)
{
    t_split ret;
    int     i;

    ret.start = ft_split_quotes(str);      // Split respecting quotes
    if (ret.start == NULL)                 // Allocation failure
    {
        ret.size = -1;                     // Error indicator
        return (ret);
    }
    i = 0;
    while (ret.start[i] != NULL)           // Count tokens
        i++;
    ret.size = i;                          // Store count
    return (ret);
}
```

**Purpose:** Creates a t_split structure from a string, respecting quote boundaries.

**t_split Structure:**
```c
typedef struct s_split
{
    char **start;    // Array of token strings
    int   size;      // Number of tokens
} t_split;
```

**Error Handling:** Returns size = -1 on allocation failure

### `ft_split_quotes(const char *s)`
**Location:** `parser/t_split_utils/ft_split_quotes.c` (lines 61-79)

```c
char **ft_split_quotes(const char *s)
{
    int     i;
    int     cnt;
    int     j;
    char    **tab;

    i = 0;
    cnt = 0;
    j = 0;
    tab = malloc(sizeof(char *) * (count_tokens(s, &i, &cnt) + 1));
    if (!tab)
        return (NULL);
    i = 0;
    while (s[i])
    {
        while (s[i] && (s[i] == ' ' || s[i] == '\t'))  // Skip whitespace
            i++;
        if (!s[i])
            break ;
        extract_token(&i, s, &tab, &j);               // Extract next token
    }
    tab[j] = NULL;                                    // Null terminate array
    return (tab);
}
```

**Purpose:** Advanced string splitting that respects quote boundaries.

**Features:**
- **Quote Awareness:** Treats quoted strings as single tokens
- **Whitespace Handling:** Splits on spaces and tabs outside quotes
- **Memory Management:** Allocates exact number of needed pointers

**Difference from Standard Split:** Spaces inside quotes don't cause token separation

### `count_tokens(const char *s, int *i, int *cnt)` (static)
**Location:** `parser/t_split_utils/ft_split_quotes.c` (lines 16-36)

```c
static int count_tokens(const char *s, int *i, int *cnt)
{
    unsigned char quote_state;

    while (s[*i])
    {
        // Skip leading whitespace
        while (s[*i] && (s[*i] == ' ' || s[*i] == '\t') && !quote_state)
            (*i)++;
        if (!s[*i])
            break ;
        (*cnt)++;                    // Found a token
        quote_state = 0;
        while (s[*i])
        {
            if (s[*i] == '\'' && !(quote_state & 2))      // Single quote toggle
                quote_state ^= 1;
            else if (s[*i] == '"' && !(quote_state & 1))  // Double quote toggle
                quote_state ^= 2;
            else if ((s[*i] == ' ' || s[*i] == '\t') && !quote_state)
                break ;                                   // End of token
            (*i)++;
        }
    }
    return (*cnt);
}
```

**Purpose:** Counts the number of tokens considering quote boundaries.

**Quote State Encoding:**
- **Bit 1 (0x1):** Inside single quotes
- **Bit 2 (0x2):** Inside double quotes
- **Bitwise Logic:** Prevents quote type conflicts

### `extract_token(int *i, const char *s, char ***tab, int *j)` (static)
**Location:** `parser/t_split_utils/ft_split_quotes.c` (lines 38-59)

```c
static void extract_token(int *i, const char *s, char ***tab, int *j)
{
    int               start;
    unsigned char     quote_state;

    start = *i;                          // Mark token beginning
    quote_state = 0;
    while (s[*i])
    {
        if (s[*i] == '\'' && !(quote_state & 2))        // Toggle single quotes
            quote_state ^= 1;
        else if (s[*i] == '"' && !(quote_state & 1))    // Toggle double quotes
            quote_state ^= 2;
        else if ((s[*i] == ' ' || s[*i] == '\t') && !quote_state)
            break ;                                     // End of token
        (*i)++;
    }
    (*tab)[(*j)++] = ft_substr(s, start, (*i) - start);  // Extract substring
}
```

**Purpose:** Extracts a single token from the string respecting quote boundaries.

**Process:**
1. **Mark Start:** Remember token beginning position
2. **Quote Tracking:** Update quote state as we scan
3. **End Detection:** Stop at unquoted whitespace or end of string
4. **Extraction:** Create substring from start to current position

### Character Counting Functions

### `countchr_not_quote(char *str, char c)`
**Location:** `parser/and_or_parser/and_or_utils.c` (line 75)

```c
int countchr_not_quote(char *str, char c)
{
    return (countchr_str(str, c) - countchr_quote(str, c));
}
```

**Purpose:** Counts occurrences of character `c` that are NOT inside quotes.

**Logic:** Total occurrences minus quoted occurrences = unquoted occurrences

### `countchr_str(char *str, char c)`
**Location:** `parser/and_or_parser/and_or_utils.c` (lines 38-51)

```c
int countchr_str(char *str, char c)
{
    int j;
    int count;

    j = 0;
    count = 0;
    if (str == NULL || *str == '\0')
        return (0);
    while (str[j] != '\0')
    {
        if (str[j] == c)        // Simple character match
            count++;
        j++;
    }
    return (count);
}
```

**Purpose:** Counts total occurrences of character `c` in string (including quoted).

**Simple Implementation:** Direct character-by-character comparison

### `countchr_quote(char *str, char c)` (static)
**Location:** `parser/and_or_parser/and_or_utils.c` (lines 53-73)

```c
static int countchr_quote(char *str, char c)
{
    int j;
    int inquote;
    int count;

    j = 0;
    count = 0;
    inquote = 0;           // Quote state tracker
    if (str == NULL || *str == '\0')
        return (0);
    while (str[j] != '\0')
    {
        inquote ^= (str[j] == '\"' && inquote != 2);      // Toggle double quotes
        inquote ^= 2 * (str[j] == '\'' && inquote != 1);  // Toggle single quotes
        if (inquote != 0 && str[j] == c)                  // Character inside quotes
            count++;
        j++;
    }
    return (count);
}
```

**Purpose:** Counts occurrences of character `c` that ARE inside quotes.

**Quote State Logic:**
- **0:** Outside all quotes
- **1:** Inside double quotes
- **2:** Inside single quotes
- **XOR Operations:** Toggle quote states safely

### Wildcard Processing

### `wildcard_handle(char *wildcard)` (static)
**Location:** `parser/wildcard_handle/wildcard_handle.c` (lines 62-87)

```c
static char *wildcard_handle(char *wildcard)
{
    t_split cur_dir;
    char    *check_list;        // '1' = match, '0' = no match
    char    *ret;
    int     i;

    i = 0;
    ret = NULL;
    cur_dir = get_cur_dir_entries(".");                     // Get all files in current dir
    check_list = (char *)ft_calloc(cur_dir.size + 1, sizeof(char));
    ft_memset(check_list, '1', cur_dir.size);              // Initially all match
    apply_filter(cur_dir, check_list, wildcard);           // Apply pattern filtering
    while (i < cur_dir.size)
    {
        if (check_list[i] == '1' && cur_dir.start[i]       // File matches pattern
            && cur_dir.start[i][0] != '\0')
            ret = ft_strjoin_sq_f(&ret, cur_dir.start[i]); // Add to result with quotes
        i++;
    }
    free(check_list);
    free_split(&cur_dir);
    return (ret);
}
```

**Purpose:** Expands a wildcard pattern to matching filenames in current directory.

**Algorithm:**
1. **Directory Scan:** Get all entries in current directory
2. **Filter Initialization:** Mark all files as potential matches
3. **Pattern Matching:** Apply wildcard filter to eliminate non-matches
4. **Result Building:** Concatenate matching files with single quotes and spaces
5. **Memory Cleanup:** Free all temporary structures

**Output Format:** `'file1' 'file2' 'file3'` (space-separated, single-quoted filenames)

**Hidden Files:** Typically excluded unless pattern explicitly starts with '.'

---

## Directory and Filesystem Functions

### `get_cur_dir_entries(const char *direc)` (static)
**Location:** `parser/wildcard_handle/wildcard_handle.c` (lines 33-58)

```c
static t_split get_cur_dir_entries(const char *direc)
{
    t_split         cur_dir_ent;
    DIR             *dir;
    int             i;
    struct dirent   *entry;

    cur_dir_ent.size = count_dir_entries(direc);                    // Count entries first
    cur_dir_ent.start = (char **)ft_calloc(cur_dir_ent.size + 1, sizeof(char *));
    if (cur_dir_ent.start == NULL)
        return (create_split(NULL, 0));                             // Error handling
    dir = opendir(direc);                                           // Open directory
    entry = readdir(dir);                                           // Read first entry
    i = 0;
    while (entry != NULL)                                           // Read all entries
    {
        if (entry->d_name[0] != '\0')                              // Valid entry name
        {
            cur_dir_ent.start[i] = ft_strdup(entry->d_name);      // Copy entry name
            i++;
        }
        entry = readdir(dir);                                       // Next entry
    }
    closedir(dir);                                                  // Close directory
    return (cur_dir_ent);
}
```

**Purpose:** Reads all entries from a directory and returns them in a t_split structure.

**Process:**
1. **Size Calculation:** First pass counts entries
2. **Memory Allocation:** Allocates array for entry pointers
3. **Directory Reading:** Uses POSIX directory functions
4. **Entry Storage:** Duplicates each entry name
5. **Cleanup:** Closes directory handle

### `count_dir_entries(const char *directory)` (static)
**Location:** `parser/wildcard_handle/wildcard_handle.c` (lines 15-30)

```c
static int count_dir_entries(const char *directory)
{
    DIR             *dir;
    struct dirent   *entry;
    int             count;

    dir = opendir(directory);               // Open directory for reading
    entry = readdir(dir);                   // Read first entry
    count = 0;
    while (entry != NULL)                   // Count all entries
    {
        if (entry->d_name[0] != '\0')      // Valid entry (non-empty name)
            count++;
        entry = readdir(dir);               // Next entry
    }
    closedir(dir);                          // Close directory
    return (count);
}
```

**Purpose:** Counts the number of valid entries in a directory for memory allocation.

**POSIX Functions Used:**
- `opendir()`: Opens directory for reading
- `readdir()`: Reads directory entries sequentially  
- `closedir()`: Closes directory handle

### POSIX Directory Functions (Library)

### `opendir(const char *name)`
**Library:** POSIX.1-2001 standard

**Purpose:** Opens directory stream for reading entries.

**Return Values:**
- **Non-NULL:** Pointer to DIR structure for use with readdir()
- **NULL:** Error occurred (check errno)

**Usage Pattern:** Always paired with closedir() for cleanup

### `readdir(DIR *dirp)`
**Library:** POSIX.1-2001 standard  

**Purpose:** Reads the next directory entry from the directory stream.

**Return Values:**
- **Non-NULL:** Pointer to struct dirent containing entry information
- **NULL:** End of directory or error

**Key Fields:**
- `d_name[]`: Filename (null-terminated string)
- `d_type`: File type (DT_REG, DT_DIR, etc.) - not always available

### `closedir(DIR *dirp)`
**Library:** POSIX.1-2001 standard

**Purpose:** Closes directory stream and frees associated resources.

**Return Values:**
- **0:** Success
- **-1:** Error occurred

---

## Wildcard Pattern Processing Functions

### `apply_filter(t_split cur_dir, char *check_list, char *wildcard)`
**Location:** `parser/wildcard_handle/wildcard_filter2.c` (lines 73-88)

```c
void apply_filter(t_split cur_dir, char *check_list, char *wildcard)
{
    t_split filter;
    char    *processed_wildcard;

    processed_wildcard = process_wildcard_pattern(wildcard);        // Handle quoted asterisks
    if (!processed_wildcard)
        return ;
    filter = create_filter(wildcard);                               // Split pattern by asterisks
    apply_filter_minlen(filter, cur_dir, check_list, processed_wildcard);  // Minimum length check
    apply_filter_start(filter, cur_dir, check_list);               // Check filename start
    apply_filter_end(filter, cur_dir, check_list);                 // Check filename end
    apply_filter_middle(filter, cur_dir, check_list);              // Check middle patterns
    free(processed_wildcard);
    free_split(&filter);
}

```

**Purpose:** Main wildcard filtering function that applies all pattern matching rules.

**Filtering Stages:**
1. **Pattern Processing:** Convert quoted asterisks to control characters
2. **Filter Creation:** Split wildcard pattern into segments
3. **Length Filter:** Eliminate files too short to match
4. **Start Filter:** Check if filename starts with first pattern
5. **End Filter:** Check if filename ends with last pattern  
6. **Middle Filter:** Verify middle patterns appear in correct order

### `process_wildcard_pattern(const char *wildcard)` (static)
**Location:** `parser/wildcard_handle/wildcard_filter2.c` (lines 44-54)

```c
static char *process_wildcard_pattern(const char *wildcard)
{
    char    *result;
    int     vars[4];        // [pos, result_pos, in_single_quote, in_double_quote]

    result = init_pattern_result(wildcard, vars);           // Allocate result buffer
    if (!result)
        return (NULL);
    process_wildcard_loop(wildcard, result, vars);          // Process each character
    result[vars[1]] = '\0';                                 // Null terminate
    return (result);
}
```

**Purpose:** Processes wildcard pattern to handle quoted asterisks correctly.

**Quote Handling:** Asterisks inside quotes become `\001` control characters

### `init_pattern_result(const char *wildcard, int *vars)`
**Location:** `parser/wildcard_handle/wildcard_quote_utils.c` (lines 40-50)

```c
char *init_pattern_result(const char *wildcard, int *vars)
{
    char    *result;

    result = malloc(ft_strlen(wildcard) + 1);               // Allocate same size as input
    if (!result)
        return (NULL);
    vars[0] = 0;    // Input position
    vars[1] = 0;    // Output position  
    vars[2] = 0;    // In single quotes flag
    vars[3] = 0;    // In double quotes flag
    return (result);
}
```

**Purpose:** Initializes pattern processing variables and allocates result buffer.

### `process_wildcard_loop(const char *wildcard, char *result, int *vars)`
**Location:** `parser/wildcard_handle/wildcard_quote_utils.c` (lines 52-67)

```c
void process_wildcard_loop(const char *wildcard, char *result, int *vars)
{
    while (wildcard[vars[0]])
    {
        if (handle_single_quote(wildcard, &vars[0], &vars[2], vars[3]))
            continue ;                                      // Skip quote processing
        if (handle_double_quote(wildcard, &vars[0], &vars[3], vars[2]))
            continue ;                                      // Skip quote processing
        if (wildcard[vars[0]] == '*' && (vars[2] || vars[3]))
            result[vars[1]++] = '\001';                    // Quoted asterisk -> control char
        else
            result[vars[1]++] = wildcard[vars[0]];         // Regular character
        vars[0]++;
    }
}
```

**Purpose:** Main loop for processing wildcard pattern character by character.

**Logic:**
- **Quote Tracking:** Maintains quote state through helper functions
- **Asterisk Conversion:** Quoted asterisks become control characters
- **Character Copy:** Regular characters copied as-is

### `handle_single_quote(const char *wildcard, int *i, int *in_single_quotes, int in_double_quotes)`
**Location:** `parser/wildcard_handle/wildcard_quote_utils.c` (lines 16-26)

```c
int handle_single_quote(const char *wildcard, int *i, 
        int *in_single_quotes, int in_double_quotes)
{
    if (wildcard[*i] == '\'' && !in_double_quotes)        // Single quote outside double quotes
    {
        *in_single_quotes = !*in_single_quotes;           // Toggle state
        (*i)++;                                            // Skip quote character
        return (1);                                        // Handled
    }
    return (0);                                            // Not handled
}
```

**Purpose:** Handles single quote state transitions in wildcard patterns.

### `handle_double_quote(const char *wildcard, int *i, int *in_double_quotes, int in_single_quotes)`
**Location:** `parser/wildcard_handle/wildcard_quote_utils.c` (lines 28-38)

```c
int handle_double_quote(const char *wildcard, int *i,
        int *in_double_quotes, int in_single_quotes)
{
    if (wildcard[*i] == '"' && !in_single_quotes)         // Double quote outside single quotes
    {
        *in_double_quotes = !*in_double_quotes;           // Toggle state
        (*i)++;                                            // Skip quote character
        return (1);                                        // Handled
    }
    return (0);                                            // Not handled
}
```

**Purpose:** Handles double quote state transitions in wildcard patterns.

### `create_filter(const char *wildcard)` (static)
**Location:** `parser/wildcard_handle/wildcard_filter2.c` (lines 56-72)

```c
static t_split create_filter(const char *wildcard)
{
    t_split     filter;
    char        *processed_wildcard;
    char        *temp;

    temp = process_wildcard_pattern(wildcard);             // Handle quotes
    if (!temp)
        return (create_split(NULL, 0));
    processed_wildcard = temp;
    if (!setup_filter_memory(&filter, processed_wildcard)) // Allocate filter array
    {
        free(processed_wildcard);
        return (create_split(NULL, 0));
    }
    extract_wildcard_parts(processed_wildcard, &filter);   // Split by asterisks
    restore_asterisks_in_filter(&filter);                  // Convert control chars back
    free(processed_wildcard);
    return (filter);
}
```

**Purpose:** Creates a filter structure by splitting wildcard pattern at asterisk boundaries.

### `setup_filter_memory(t_split *filter, char *processed_wildcard)`
**Location:** `parser/wildcard_handle/wildcard_filter2_utils.c` (lines 30-38)

```c
int setup_filter_memory(t_split *filter, char *processed_wildcard)
{
    filter->size = calculate_filter_size(processed_wildcard);       // Count segments
    filter->start = (char **)ft_calloc(filter->size + 1, sizeof(char *));
    if (filter->start == NULL)
        return (0);                                                 // Allocation failed
    return (1);                                                     // Success
}
```

**Purpose:** Allocates memory for filter segments based on asterisk count.

### `calculate_filter_size(char *processed_wildcard)`
**Location:** `parser/wildcard_handle/wildcard_filter2_utils.c` (lines 40-54)

```c
int calculate_filter_size(char *processed_wildcard)
{
    int size;
    int i;

    if (ft_strlen(processed_wildcard) == 0)
        size = 1;                                          // Empty pattern = 1 segment
    else
        size = 1;                                          // At least 1 segment
    i = 0;
    while (processed_wildcard[i])
    {
        if (processed_wildcard[i] == '*')                  // Each asterisk adds segment
            size++;
        i++;
    }
    return (size);
}
```

**Purpose:** Calculates number of filter segments by counting asterisks.

**Logic:** N asterisks create N+1 segments (before, between, after)

### `extract_wildcard_parts(char *processed_wildcard, t_split *filter)` (static)
**Location:** `parser/wildcard_handle/wildcard_filter2.c` (lines 15-42)

```c
static void extract_wildcard_parts(char *processed_wildcard, t_split *filter)
{
    int     indx[3];        // [start_pos, end_pos, segment_index]
    char    *temp_substr;

    ft_memset(indx, 0, sizeof(int) * 3);
    while (indx[0] <= (int)ft_strlen(processed_wildcard))
    {
        // Find next asterisk or end of string
        while (processed_wildcard[indx[1]] != '\0' 
            && processed_wildcard[indx[1]] != '*')
            indx[1]++;
        
        if (indx[0] != indx[1])                           // Non-empty segment
        {
            temp_substr = ft_substr(processed_wildcard, indx[0], indx[1] - indx[0]);
            filter->start[indx[2]] = temp_substr;
            indx[2]++;
        }
        else                                              // Empty segment
        {
            filter->start[indx[2]] = ft_strdup("");
            indx[2]++;
        }
        indx[0] = indx[1] + 1;                           // Skip asterisk
        indx[1] = indx[0];
    }
}
```

**Purpose:** Splits processed wildcard into segments separated by asterisks.

### `restore_asterisks_in_filter(t_split *filter)`
**Location:** `parser/wildcard_handle/wildcard_filter2_utils.c` (lines 16-28)

```c
void restore_asterisks_in_filter(t_split *filter)
{
    int i;
    int j;

    i = -1;
    while (++i < filter->size && filter->start[i])       // For each filter segment
    {
        j = -1;
        while (filter->start[i][++j])                     // For each character
            if (filter->start[i][j] == '\001')            // Control character
                filter->start[i][j] = '*';                // Restore asterisk
    }
}
```

**Purpose:** Converts control characters back to asterisks in filter segments.

**Why Needed:** Quoted asterisks were temporarily converted to avoid splitting

---

## Wildcard Filter Application Functions

### `apply_filter_minlen(t_split filter, t_split cur_dir, char *check_list, char *wildcard)`
**Location:** `parser/wildcard_handle/wildcard_filter.c` (lines 66-75)

```c
void apply_filter_minlen(t_split filter, t_split cur_dir,
                        char *check_list, char *wildcard)
{
    int     i;
    int     minlen;

    i = 0;
    minlen = ft_strlen(wildcard) - filter.size + 1;        // Calculate minimum required length
    while (i < cur_dir.size)
    {
        if ((int)ft_strlen(cur_dir.start[i]) < minlen)     // File too short to match
            check_list[i] = '0';                           // Mark as non-matching
        i++;
    }
}
```

**Purpose:** Eliminates files that are too short to possibly match the wildcard pattern.

**Algorithm:** Minimum length = wildcard length - number of asterisks + 1

**Logic:** If a file is shorter than the minimum possible match length, it cannot match the pattern regardless of asterisk placement.

### `apply_filter_start(t_split filter, t_split cur_dir, char *check_list)`
**Location:** `parser/wildcard_handle/wildcard_filter.c` (lines 18-41)

```c
void apply_filter_start(t_split filter, t_split cur_dir, char *check_list)
{
    int i;

    i = 0;
    if (filter.start[0][0] == '\0')                        // Pattern starts with asterisk
    {
        while (i < cur_dir.size)
        {
            if (cur_dir.start[i][0] == '.')                // Hide dot files
                check_list[i] = '0';
            i++;
        }
    }
    else                                                   // Pattern has specific start
    {
        while (i < cur_dir.size)
        {
            if (ft_strncmp(filter.start[0], cur_dir.start[i],
                    ft_strlen(filter.start[0])))           // Start doesn't match
                check_list[i] = '0';
            i++;
        }
    }
}
```

**Purpose:** Filters files based on whether they start with the required prefix.

**Hidden File Handling:** When pattern starts with asterisk, automatically excludes dot files (Unix convention)

**Prefix Matching:** Uses `ft_strncmp` to check if filename starts with first filter segment

### `apply_filter_end(t_split filter, t_split cur_dir, char *check_list)`
**Location:** `parser/wildcard_handle/wildcard_filter.c` (lines 43-64)

```c
void apply_filter_end(t_split filter, t_split cur_dir, char *check_list)
{
    int     i;
    char    *filter_end;

    filter_end = filter.start[filter.size - 1];           // Last filter segment
    i = 0;
    while (i < cur_dir.size && check_list[i] == '0')      // Skip already filtered files
        i++;
    if (filter.start[filter.size - 1][0] != '\0')         // Pattern has specific ending
    {
        while (i < cur_dir.size)
        {
            if (ft_strrcmp(filter_end, cur_dir.start[i]))  // End doesn't match
                check_list[i] = '0';
            i++;
            while (check_list[i] == '0')                   // Skip to next valid file
                i++;
        }
    }
}
```

**Purpose:** Filters files based on whether they end with the required suffix.

**Optimization:** Skips files already marked as non-matching from previous filters

**Suffix Matching:** Uses custom `ft_strrcmp` function to check filename endings

### `apply_filter_middle(t_split filter, t_split cur_dir, char *check_list)`
**Location:** `parser/wildcard_handle/wildcard_filter.c` (lines 106-118)

```c
void apply_filter_middle(t_split filter, t_split cur_dir, char *check_list)
{
    int     i;

    i = 0;
    if (filter.size < 3)                                   // Need at least 3 segments for middle
        return ;
    while (i < cur_dir.size)
    {
        if (check_list[i] != '0')                          // Only check unfiltered files
            apply_filter_midsin(filter, cur_dir.start[i], &(check_list[i]));
        i++;
    }
}
```

**Purpose:** Applies middle pattern filtering to remaining files.

**Prerequisite:** Only applies if pattern has middle segments (3+ filter segments)

**Delegation:** Calls `apply_filter_midsin` for each individual file check

### `apply_filter_midsin(t_split filter, char *cur_dir_i, char *check_list_i)` (static)
**Location:** `parser/wildcard_handle/wildcard_filter.c` (lines 77-104)

```c
static void apply_filter_midsin(t_split filter, char *cur_dir_i,
                                char *check_list_i)
{
    int     filter_i;
    char    *index;

    filter_i = 1;                                          // Start from second segment
    index = &(cur_dir_i[ft_strlen(filter.start[0])]);     // Skip start prefix
    while (filter_i < filter.size - 1)                    // Process middle segments
    {
        index = (ft_strnstr(index, filter.start[filter_i], ft_strlen(index)
                    - ft_strlen(filter.start[filter.size - 1])));  // Find segment
        if (index == NULL)                                 // Segment not found
        {
            *check_list_i = '0';                          // Mark as non-matching
            return ;
        }
        if (ft_strlen(index) > ft_strlen(filter.start[filter_i]))
            index = &(index[ft_strlen(filter.start[filter_i])]);  // Move past found segment
        else
            index = &(index[ft_strlen(index)]);           // End of string
        filter_i++;
    }
}
```

**Purpose:** Checks if all middle pattern segments appear in correct order within a filename.

**Algorithm:**
1. **Start Position:** Skip the already-matched prefix
2. **Sequential Search:** Find each middle segment in order
3. **Boundary Check:** Ensure enough space remains for suffix
4. **Position Update:** Move search position past each found segment

**Failure Condition:** If any middle segment is not found in the remaining string

### `ft_strjoin_sq_f(char **to_free, const char *to_add)`
**Location:** `parser/wildcard_handle/wildcard_utils.c` (lines 34-58)

```c
char *ft_strjoin_sq_f(char **to_free, const char *to_add)
{
    char    *ret;
    int     ret_size;

    if (*to_free == NULL)                                  // First file
    {
        ret_size = ft_strlen(to_add) + 3;                  // Space for 'filename'
        ret = (char *)ft_calloc(ret_size, sizeof(char));
        if (ret == NULL)
            return (NULL);
        ft_strlcpy(ret, "'", ret_size);                    // Start with quote
    }
    else                                                   // Additional files
    {
        ret_size = ft_strlen(*to_free) + ft_strlen(to_add) + 4;  // Space for " 'filename'"
        ret = (char *)ft_calloc(ret_size, sizeof(char));
        if (ret == NULL)
            return (NULL);
        ft_strlcpy(ret, *to_free, ret_size);              // Copy existing
        ft_strlcat(ret, " '", ret_size);                  // Add space and quote
        free(*to_free);                                    // Free old string
    }
    ft_strlcat(ret, to_add, ret_size);                    // Add filename
    ft_strlcat(ret, "'", ret_size);                       // Close quote
    return (ret);
}
```

**Purpose:** Joins filenames with single quotes and spaces for shell command construction.

**Memory Management:** Frees the old string (`*to_free`) and returns new concatenated string

**Output Format:** Creates `'file1' 'file2' 'file3'` format for shell expansion

**Use Case:** Building space-separated, quoted filename lists for command execution

---

## Parser System

### `parser_and_or(t_shell *shell, t_split split)`

**Note:** This function and the complete parsing system are documented in detail in **[PARSER_DOCUMENTATION.md](PARSER_DOCUMENTATION.md)**.

The parser handles:
- Logical operators (`&&`, `||`)
- Parentheses grouping
- Command precedence
- Error handling and syntax validation

---

## Memory Management Functions

### `free_split(t_split *split)`
**Location:** `parser/t_split_utils/t_split_utils.c` (lines 18-36)

```c
void free_split(t_split *split)
{
    int i;

    if (split == NULL || split->start == NULL)            // Safety checks
        return ;
    i = 0;
    while (i < split->size && split->start[i] != NULL)    // Free each string
    {
        free(split->start[i]);
        split->start[i] = NULL;                           // Prevent double-free
        i++;
    }
    free(split->start);                                   // Free pointer array
    split->start = NULL;                                  // Reset structure
    split->size = 0;
}
```

**Purpose:** Safely deallocates all memory associated with a t_split structure.

**Safety Features:**
- Null pointer checks before freeing
- Sets pointers to NULL after freeing to prevent double-free errors
- Resets structure fields to safe values

**Usage Pattern:** Called after processing any t_split structure to prevent memory leaks

### `free_env(t_shell *shell)`
**Location:** `main/environment.c` (lines 43-54)

```c
void free_env(t_shell *shell)
{
    int i;

    if (shell->envp == NULL)                              // Nothing to free
        return ;
    i = 0;
    while (shell->envp[i] != NULL)                        // Free each environment string
    {
        free(shell->envp[i]);
        i++;
    }
    free(shell->envp);                                    // Free pointer array
    shell->envp = NULL;                                   // Reset to safe state
}
```

**Purpose:** Deallocates the dynamically allocated environment variables array.

**Safety Features:**
- Null check prevents errors if envp was never allocated
- Frees each individual environment string
- Frees the pointer array itself
- Resets pointer to NULL to prevent dangling references

**Usage:** Called during shell cleanup (exit) and error handling

---

## Summary

This expanded documentation now covers the complete input processing pipeline:

1. **Quote Validation:** Ensuring balanced quotes before processing
2. **Operator Separation:** Adding spaces around shell operators for proper tokenization  
3. **Wildcard Expansion:** Converting glob patterns to matching filenames
4. **Advanced Tokenization:** Splitting strings while respecting quote boundaries
5. **Character Analysis:** Counting characters with quote-awareness
6. **Pattern Matching:** File globbing with sophisticated filtering

Each function is designed with specific responsibilities, clear interfaces, and robust error handling to create a reliable shell implementation that closely mimics bash behavior.
