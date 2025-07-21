#!/bin/bash

# Memory Leak Testing Script for Minishell
# This script tests various features in isolation to identify memory leaks

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

VALGRIND_CMD="valgrind --leak-check=full --errors-for-leak-kinds=all --show-leak-kinds=all --track-origins=yes --suppressions=/dev/null"
MINISHELL="./minishell"
LOG_DIR="memory_leak_logs"

# Create log directory
mkdir -p "$LOG_DIR"

echo -e "${BLUE}=== MINISHELL MEMORY LEAK TESTING ===${NC}"
echo "Testing with valgrind to detect all memory leaks including 'still reachable'"
echo

# Function to run a test with valgrind
run_leak_test() {
    local test_name="$1"
    local test_commands="$2"
    local log_file="$LOG_DIR/${test_name// /_}.log"
    
    echo -n -e "${YELLOW}Testing: $test_name${NC} ... "
    
    # Run the test
    echo -e "$test_commands\nexit" | $VALGRIND_CMD $MINISHELL > /dev/null 2> "$log_file"
    
    # Check for leaks
    local definitely_lost=$(grep "definitely lost:" "$log_file" | awk '{print $4}')
    local indirectly_lost=$(grep "indirectly lost:" "$log_file" | awk '{print $4}')
    local possibly_lost=$(grep "possibly lost:" "$log_file" | awk '{print $4}')
    local still_reachable=$(grep "still reachable:" "$log_file" | awk '{print $4}')
    
    # Remove commas from numbers
    definitely_lost=${definitely_lost//,/}
    indirectly_lost=${indirectly_lost//,/}
    possibly_lost=${possibly_lost//,/}
    still_reachable=${still_reachable//,/}
    
    # Check if all heap blocks were freed
    if grep -q "All heap blocks were freed" "$log_file"; then
        echo -e "${GREEN}PASS${NC} - All heap blocks freed"
    elif [[ "$definitely_lost" == "0" && "$indirectly_lost" == "0" && "$possibly_lost" == "0" && "$still_reachable" == "0" ]]; then
        echo -e "${GREEN}PASS${NC} - No leaks detected"
    else
        echo -e "${RED}FAIL${NC}"
        echo "  Definitely lost: $definitely_lost bytes"
        echo "  Indirectly lost: $indirectly_lost bytes"
        echo "  Possibly lost: $possibly_lost bytes"
        echo "  Still reachable: $still_reachable bytes"
        echo "  Check log: $log_file"
    fi
}

# Test 1: Basic Commands
echo -e "\n${BLUE}1. BASIC COMMANDS${NC}"
run_leak_test "empty_command" ""
run_leak_test "simple_echo" "echo hello"
run_leak_test "echo_with_args" "echo hello world test"
run_leak_test "echo_with_n_flag" "echo -n test"
run_leak_test "pwd_command" "pwd"
run_leak_test "env_command" "env"

# Test 2: Variable Expansion
echo -e "\n${BLUE}2. VARIABLE EXPANSION${NC}"
run_leak_test "simple_var_expansion" 'echo $USER'
run_leak_test "multiple_var_expansion" 'echo $HOME $USER $PATH'
run_leak_test "nonexistent_var" 'echo $NONEXISTENT'
run_leak_test "exit_status_var" 'echo $?'
run_leak_test "complex_var_expansion" 'echo "$USER:$HOME:$PATH"'
run_leak_test "var_in_quotes" 'echo "Hello $USER"'
run_leak_test "empty_var_expansion" 'echo $EMPTY_VAR'

# Test 3: Quote Processing
echo -e "\n${BLUE}3. QUOTE PROCESSING${NC}"
run_leak_test "single_quotes" "echo 'hello world'"
run_leak_test "double_quotes" 'echo "hello world"'
run_leak_test "mixed_quotes" 'echo "hello '\''world'\''"'
run_leak_test "quotes_with_vars" 'echo "$USER '\''test'\''"'
run_leak_test "empty_quotes" 'echo ""'
run_leak_test "nested_quotes" 'echo "test '\''$USER'\'' test"'

# Test 4: Pipes
echo -e "\n${BLUE}4. PIPES${NC}"
run_leak_test "simple_pipe" "echo hello | cat"
run_leak_test "multiple_pipes" "echo hello | cat | cat | cat"
run_leak_test "pipe_with_builtin" "echo test | grep test"
run_leak_test "complex_pipe" "ls | grep minishell | wc -l"
run_leak_test "pipe_with_vars" 'echo $USER | cat'

# Test 5: Redirections
echo -e "\n${BLUE}5. REDIRECTIONS${NC}"
run_leak_test "output_redirect" "echo test > /tmp/test_out.txt"
run_leak_test "input_redirect" "cat < /etc/passwd | head -1"
run_leak_test "append_redirect" "echo test >> /tmp/test_out.txt"
run_leak_test "here_doc" 'cat << EOF
Hello
World
EOF'
run_leak_test "here_doc_with_vars" 'cat << EOF
$USER
$HOME
EOF'
run_leak_test "multiple_redirects" "echo test > /tmp/out1 > /tmp/out2"

# Test 6: Export and Unset
echo -e "\n${BLUE}6. EXPORT AND UNSET${NC}"
run_leak_test "simple_export" "export TEST=value"
run_leak_test "export_and_use" "export TEST=hello && echo \$TEST"
run_leak_test "multiple_exports" "export A=1 B=2 C=3"
run_leak_test "unset_var" "export TEST=value && unset TEST"
run_leak_test "export_empty" "export EMPTY="
run_leak_test "export_with_spaces" 'export TEST="hello world"'

# Test 7: CD Command
echo -e "\n${BLUE}7. CD COMMAND${NC}"
run_leak_test "cd_home" "cd ~"
run_leak_test "cd_relative" "cd .."
run_leak_test "cd_absolute" "cd /tmp"
run_leak_test "cd_oldpwd" "cd /tmp && cd -"
run_leak_test "cd_error" "cd /nonexistent 2>/dev/null"

# Test 8: Logical Operators (Bonus)
echo -e "\n${BLUE}8. LOGICAL OPERATORS${NC}"
run_leak_test "and_operator" "echo test && echo success"
run_leak_test "or_operator" "false || echo fallback"
run_leak_test "mixed_operators" "echo test && echo middle || echo end"

# Test 9: Parentheses (Bonus)
echo -e "\n${BLUE}9. PARENTHESES${NC}"
run_leak_test "simple_parentheses" "(echo test)"
run_leak_test "nested_parentheses" "((echo nested))"
run_leak_test "parentheses_with_pipe" "(echo test | cat)"

# Test 10: Wildcards (Bonus)
echo -e "\n${BLUE}10. WILDCARDS${NC}"
run_leak_test "simple_wildcard" "echo *.txt"
run_leak_test "wildcard_no_match" "echo *.nonexistent"
run_leak_test "wildcard_in_quotes" 'echo "*.txt"'

# Test 11: Complex Commands
echo -e "\n${BLUE}11. COMPLEX COMMANDS${NC}"
run_leak_test "complex_1" 'export VAR=test && echo $VAR | cat > /tmp/out'
run_leak_test "complex_2" '(echo test && echo $USER) | grep test'
run_leak_test "complex_3" 'cd /tmp && pwd && cd - > /dev/null'

# Test 12: Error Cases
echo -e "\n${BLUE}12. ERROR CASES${NC}"
run_leak_test "syntax_error_pipe" "| echo test"
run_leak_test "syntax_error_redirect" "> > test"
run_leak_test "unclosed_quote" 'echo "unclosed'
run_leak_test "invalid_command" "nonexistent_command"

# Test 13: Edge Cases
echo -e "\n${BLUE}13. EDGE CASES${NC}"
run_leak_test "empty_input" ""
run_leak_test "only_spaces" "   "
run_leak_test "very_long_input" "echo $(printf 'a%.0s' {1..1000})"
run_leak_test "many_args" "echo $(seq 1 100)"

# Summary
echo -e "\n${BLUE}=== SUMMARY ===${NC}"
echo "All test logs saved in: $LOG_DIR/"
echo "To view detailed leak information, check individual log files."
echo
echo "Common leak sources to check:"
echo "1. Variable expansion not freeing temporary strings"
echo "2. Wildcard expansion not freeing pattern results"
echo "3. Quote processing leaving duplicated strings"
echo "4. Pipeline file descriptors not closed"
echo "5. Environment variables not freed on exit"
echo "6. Here document temporary buffers"
echo "7. Command argument arrays not fully freed"

# Cleanup temporary files
rm -f /tmp/test_out.txt /tmp/out /tmp/out1 /tmp/out2

echo -e "\n${GREEN}Testing complete!${NC}" 