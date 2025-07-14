#!/bin/bash

# Comprehensive test suite for minishell
# Tests all implemented features

echo "=== MINISHELL COMPREHENSIVE TEST SUITE ==="
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

PASSED=0
FAILED=0
TOTAL=0

test_command() {
    local test_name="$1"
    local command="$2"
    local expected="$3"
    local timeout_duration="${4:-3}"
    
    TOTAL=$((TOTAL + 1))
    echo -n "Testing $test_name... "
    
    # Run the command with timeout
    local result
    result=$(echo "$command" | timeout $timeout_duration ./minishell 2>&1)
    local exit_code=$?
    
    if [ $exit_code -eq 124 ]; then
        echo -e "${RED}TIMEOUT${NC}"
        FAILED=$((FAILED + 1))
        return 1
    fi
    
    # Check if expected output is in the result
    if echo "$result" | grep -q "$expected"; then
        echo -e "${GREEN}PASS${NC}"
        PASSED=$((PASSED + 1))
        return 0
    else
        echo -e "${RED}FAIL${NC}"
        echo "  Expected: $expected"
        echo "  Got: $result"
        FAILED=$((FAILED + 1))
        return 1
    fi
}

# Make sure minishell exists
if [ ! -f "./minishell" ]; then
    echo "Error: minishell binary not found!"
    exit 1
fi

echo "1. BASIC FUNCTIONALITY TESTS"
echo "=============================="

test_command "Basic echo" "echo hello\nexit" "hello"
test_command "Echo with -n flag" "echo -n test\nexit" "testminishell >"
test_command "Multiple echo -n flags" "echo -nnn hello\nexit" "hellominishell >"
test_command "Echo without -n flag after args" "echo hello -n\nexit" "hello -n"

echo
echo "2. BUILT-IN COMMANDS TESTS"
echo "=========================="

test_command "pwd command" "pwd\nexit" "$(pwd)"
test_command "cd and pwd" "cd ..\npwd\nexit" "$(dirname $(pwd))"
test_command "export and echo" "export TESTVAR=hello\necho \$TESTVAR\nexit" "hello"
test_command "env command" "env\nexit" "PATH="
test_command "unset command" "export TESTVAR=hello\nunset TESTVAR\necho \$TESTVAR\nexit" "minishell >"

echo
echo "3. ENVIRONMENT VARIABLE TESTS"
echo "============================="

test_command "HOME variable" "echo \$HOME\nexit" "$HOME"
test_command "USER variable" "echo \$USER\nexit" "$USER"
test_command "Exit status \$?" "echo \$?\nexit" "0"
test_command "Non-existent variable" "echo \$NONEXISTENT\nexit" "minishell >"

echo
echo "4. EXTERNAL COMMAND TESTS"
echo "========================="

test_command "ls command" "ls\nexit" "minishell"
test_command "ls with flags" "ls -l\nexit" "total"
test_command "cat with file" "echo test > /tmp/testfile\ncat /tmp/testfile\nexit" "test"
test_command "which command" "which ls\nexit" "/bin/ls"

echo
echo "5. ERROR HANDLING TESTS"
echo "======================="

test_command "Non-existent command" "nonexistentcommand\nexit" "command not found"
test_command "cd to non-existent dir" "cd /nonexistent\nexit" "No such file or directory"

echo
echo "6. SIGNAL HANDLING TESTS"
echo "========================"

# Note: Signal tests are more complex and may need manual testing
echo "Signal tests require manual verification:"
echo "- Ctrl+C should show new prompt"
echo "- Ctrl+D should exit shell"  
echo "- Ctrl+\\ should do nothing in interactive mode"

echo
echo "7. QUOTE HANDLING TESTS"
echo "======================="

test_command "Single quotes" "echo 'hello world'\nexit" "hello world"
test_command "Double quotes" "echo \"hello world\"\nexit" "hello world"
test_command "Double quotes with variable" "export VAR=test\necho \"\$VAR\"\nexit" "test"
test_command "Single quotes with variable" "export VAR=test\necho '\$VAR'\nexit" "\$VAR"

echo
echo "=== TEST SUMMARY ==="
echo "Total tests: $TOTAL"
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$FAILED${NC}"

if [ $FAILED -eq 0 ]; then
    echo -e "\n${GREEN}ALL TESTS PASSED!${NC}"
    exit 0
else
    echo -e "\n${RED}SOME TESTS FAILED${NC}"
    exit 1
fi
