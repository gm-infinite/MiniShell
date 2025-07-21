# Parentheses Parsing Issues Analysis

## Current Problems Identified

### 1. **Character Loss in Tokenization**
- **Issue**: Commands like `wc` become `w`, `echo` becomes `ech`
- **Root Cause**: `ft_memmove` in parentheses processing is using incorrect lengths
- **Status**: Partially fixed but still occurring in complex scenarios

### 2. **Multiple Syntax Errors**
- **Issue**: Complex nested parentheses generate multiple "syntax error: unexpected end of file"
- **Root Cause**: Parser is processing the same input multiple times
- **Impact**: Causes incorrect parsing and execution

### 3. **Incorrect Command Resolution**  
- **Issue**: Commands in parentheses are not being resolved correctly
- **Root Cause**: Tokenization corruption affects command lookup

### 4. **Exit Command Echo**
- **Issue**: "exit" appears in stdout when it shouldn't
- **Root Cause**: Shell processes exit command and echoes it

## Specific Failing Scenarios

1. `((echo 1) | (echo 2) | (echo 3 | (echo 4)))`
   - Expected: `4`
   - Actual: Syntax errors + "ech: command not found"

2. `echo 1 | (echo 2 || echo 3 && echo 4) || echo 5 | echo 6`
   - Expected: `2` and `4`
   - Actual: Syntax error + `6`

3. `echo 1 | (grep 1) | cat | (wc -l)`
   - Expected: `1`
   - Actual: Syntax error + "w: invalid option"

## Root Cause Analysis

The main issue is in the **recursive parentheses processing**:

1. **`cut_out_par` function** - Modifies strings in place incorrectly
2. **`remove_opening_paren` function** - Memory corruption during string manipulation
3. **Multiple processing passes** - Same input processed multiple times causing corruption
4. **Insufficient bounds checking** - Buffer overruns in string operations

## Proposed Solution Strategy

### Immediate Fixes Needed:

1. **Fix String Manipulation**
   - Correct all `ft_memmove` calls to use proper lengths
   - Add null termination after all string operations
   - Add bounds checking

2. **Improve Parentheses Logic**
   - Avoid multiple processing of same input
   - Better state management during parsing
   - Cleaner separation of concerns

3. **Debug Output Cleanup**
   - Remove bash prefixes from error messages
   - Fix exit command echoing

### Long-term Improvements:

1. **Rewrite Parentheses Parser**
   - Use a more robust recursive descent parser
   - Better error recovery
   - Cleaner state management

2. **Improve Testing**
   - Add unit tests for tokenization
   - Test parentheses processing in isolation
   - Comprehensive bash compatibility tests

## Current Status

- ✅ Fixed basic `$?` segfaults
- ✅ Fixed dot command exit codes  
- ⚠️ Partially fixed string corruption (still issues in complex cases)
- ❌ Complex parentheses scenarios still failing
- ❌ Multiple syntax errors still occurring

## Next Steps

1. Focus on the three specific failing scenarios
2. Add comprehensive debugging to understand exact failure points
3. Implement targeted fixes for string corruption
4. Test incrementally with simpler cases first 