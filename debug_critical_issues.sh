#!/bin/bash

# Debug Critical Issues in Minishell
# This script tests the specific issues mentioned by the user

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== DEBUGGING CRITICAL MINISHELL ISSUES ===${NC}"
echo

# Function to run a test and compare with bash
run_comparison_test() {
    local test_name="$1"
    local test_cmd="$2"
    local timeout_duration="${3:-5}"
    
    echo -e "\n${YELLOW}Testing: $test_name${NC}"
    echo "Command: $test_cmd"
    
    # Run with minishell
    echo -e "$test_cmd\nexit" > test_input.tmp
    timeout $timeout_duration ./minishell < test_input.tmp > mini_out.tmp 2> mini_err.tmp
    local mini_exit=$?
    
    # Run with bash
    echo -e "$test_cmd\nexit" > test_input.tmp
    timeout $timeout_duration bash < test_input.tmp > bash_out.tmp 2> bash_err.tmp
    local bash_exit=$?
    
    echo "Minishell exit: $mini_exit"
    echo "Bash exit: $bash_exit"
    
    echo "Minishell stdout: $(cat mini_out.tmp | grep -v "minishell >" | head -3)"
    echo "Bash stdout: $(cat bash_out.tmp | head -3)"
    
    echo "Minishell stderr: $(cat mini_err.tmp | head -3)"
    echo "Bash stderr: $(cat bash_err.tmp | head -3)"
    
    if [ "$mini_exit" = "$bash_exit" ]; then
        echo -e "${GREEN}✓ Exit codes match${NC}"
    else
        echo -e "${RED}✗ Exit codes differ${NC}"
    fi
    
    rm -f test_input.tmp mini_out.tmp mini_err.tmp bash_out.tmp bash_err.tmp
    echo "---"
}

# Test problematic parentheses scenarios
echo -e "${BLUE}1. PARENTHESES AND LOGICAL OPERATORS${NC}"

run_comparison_test "Complex nested parentheses with pipes" "((echo 1) | (echo 2) | (echo 3 | (echo 4)))"

run_comparison_test "Logical operators with pipes" "echo 1 | (echo 2 || echo 3 && echo 4) || echo 5 | echo 6"

run_comparison_test "Pipe with parentheses and wc" "echo 1 | (grep 1) | cat | (wc -l)"

# Test exit status variable issues
echo -e "\n${BLUE}2. EXIT STATUS VARIABLE (\$?) ISSUES${NC}"

run_comparison_test "Basic exit status" "echo \$?"

run_comparison_test "Exit status after command" "/bin/echo 42 && echo \$?"

run_comparison_test "Exit status after failed command" "/bin/cat nonexistent && echo \$?"

run_comparison_test "Exit status in quotes" 'echo "$?"'

run_comparison_test "Exit status with text" 'echo $?"42"'

run_comparison_test "Exit status in single quotes" "echo '\$?'"

run_comparison_test "Exit status in complex quotes" "echo \"'\$?'\""

# Test problematic scenarios that cause segfaults
echo -e "\n${BLUE}3. SEGFAULT-CAUSING SCENARIOS${NC}"

echo -e "${YELLOW}Testing potentially segfault-causing commands:${NC}"

# Test expr with exit status
run_comparison_test "expr with exit status" "expr \$? + \$?" 10

# Test exit status in variable names
run_comparison_test "Exit status in export" "export TES\$?T=123" 10

run_comparison_test "Exit status in unset" "unset TES\$?T" 10

run_comparison_test "Export with exit status as name" "export \$?" 10

# Test dot commands
echo -e "\n${BLUE}4. DOT COMMAND ISSUES${NC}"

run_comparison_test "Single dot command" "."

run_comparison_test "Double dot command" ".."

run_comparison_test "Quoted dot" '"."'

# Test path-like commands
echo -e "\n${BLUE}5. PATH-LIKE COMMAND ISSUES${NC}"

run_comparison_test "Slash command" "/ls"

run_comparison_test "Dot slash" "./ls"

run_comparison_test "Quoted dot slash" '"./"ls'

run_comparison_test "Mixed quotes with dot slash" '"./""ls"'

run_comparison_test "Quote combination" '"./"'\'''\''ls'

echo -e "\n${BLUE}=== DEBUGGING COMPLETE ===${NC}"
echo "Check the output above to identify specific issues." 