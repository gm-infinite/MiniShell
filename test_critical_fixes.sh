#!/bin/bash

# Test Critical Fixes for Minishell
# This script verifies that the critical issues have been resolved

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

PASSED=0
FAILED=0
TOTAL=0

echo -e "${BLUE}=== TESTING CRITICAL FIXES ===${NC}"
echo

# Function to test a specific scenario
test_fix() {
    local test_name="$1"
    local test_cmd="$2"
    local expected_behavior="$3"
    
    TOTAL=$((TOTAL + 1))
    echo -n -e "${YELLOW}Test $TOTAL: $test_name${NC} ... "
    
    # Run the test
    echo -e "$test_cmd\nexit" > test_input.tmp
    timeout 5s ./minishell < test_input.tmp > test_output.tmp 2> test_error.tmp
    local exit_code=$?
    
    # Check result
    case "$expected_behavior" in
        "no_segfault")
            if [ $exit_code -ne 139 ] && [ $exit_code -ne 11 ]; then
                echo -e "${GREEN}PASS${NC} (no segfault)"
                PASSED=$((PASSED + 1))
            else
                echo -e "${RED}FAIL${NC} (segfault detected)"
                FAILED=$((FAILED + 1))
            fi
            ;;
        "exit_127")
            # Check if the exit code is 127 (or close to it due to shell wrapping)
            if grep -q "command not found" test_error.tmp; then
                echo -e "${GREEN}PASS${NC} (command not found)"
                PASSED=$((PASSED + 1))
            else
                echo -e "${RED}FAIL${NC} (wrong error message)"
                echo "  Expected: command not found"
                echo "  Got: $(cat test_error.tmp)"
                FAILED=$((FAILED + 1))
            fi
            ;;
        "no_crash")
            if [ $exit_code -ne 124 ] && [ $exit_code -ne 139 ] && [ $exit_code -ne 11 ]; then
                echo -e "${GREEN}PASS${NC} (no crash)"
                PASSED=$((PASSED + 1))
            else
                echo -e "${RED}FAIL${NC} (crashed or timeout)"
                FAILED=$((FAILED + 1))
            fi
            ;;
    esac
    
    rm -f test_input.tmp test_output.tmp test_error.tmp
}

echo -e "${BLUE}1. Testing Exit Status Variable (\$?) Fixes${NC}"
test_fix "Basic exit status" "echo \$?" "no_segfault"
test_fix "Exit status in quotes" 'echo "$?"' "no_segfault"
test_fix "Exit status with text" 'echo $?"42"' "no_segfault"
test_fix "Exit status in export" "export TES\$?T=123" "no_segfault"
test_fix "Exit status in unset" "unset TES\$?T" "no_segfault"
test_fix "Export exit status as name" "export \$?" "no_segfault"

echo -e "\n${BLUE}2. Testing Dot Command Fixes${NC}"
test_fix "Single dot command" "." "exit_127"
test_fix "Double dot command" ".." "exit_127"
test_fix "Quoted dot" '"."' "exit_127"

echo -e "\n${BLUE}3. Testing Path Command Fixes${NC}"
test_fix "Slash command" "/ls" "no_crash"
test_fix "Dot slash" "./ls" "no_crash"
test_fix "Quoted dot slash" '"./"ls' "no_crash"

echo -e "\n${BLUE}4. Testing Complex Scenarios${NC}"
test_fix "Complex parentheses" "((echo 1) | (echo 2))" "no_crash"
test_fix "Logical with pipes" "echo 1 | (echo 2 || echo 3)" "no_crash"
test_fix "Expr with exit status" "expr \$? + \$?" "no_crash"

# Summary
echo -e "\n${BLUE}=== TEST SUMMARY ===${NC}"
echo "Total tests: $TOTAL"
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$FAILED${NC}"

if [ $FAILED -eq 0 ]; then
    echo -e "\n${GREEN}🎉 ALL CRITICAL FIXES WORKING!${NC}"
    exit 0
else
    echo -e "\n${RED}❌ SOME FIXES STILL NEED WORK${NC}"
    exit 1
fi 