#!/bin/bash

# Bash Compatibility Test for Complex Scenarios
# This script compares minishell behavior with bash exactly

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

PASSED=0
FAILED=0
TOTAL=0

echo -e "${BLUE}=== BASH COMPATIBILITY TEST FOR COMPLEX SCENARIOS ===${NC}"
echo

# Function to compare minishell and bash behavior exactly
compare_with_bash() {
    local test_name="$1"
    local test_cmd="$2"
    local timeout_duration="${3:-10}"
    
    TOTAL=$((TOTAL + 1))
    echo -e "\n${YELLOW}Test $TOTAL: $test_name${NC}"
    echo "Command: $test_cmd"
    echo "----------------------------------------"
    
    # Run with minishell
    echo -e "$test_cmd\nexit" > test_input.tmp
    timeout $timeout_duration ./minishell < test_input.tmp > mini_stdout.tmp 2> mini_stderr.tmp
    local mini_exit=$?
    
    # Clean minishell output (remove prompt)
    cat mini_stdout.tmp | grep -v "minishell >" > mini_stdout_clean.tmp
    
    # Run with bash
    echo -e "$test_cmd\nexit" > test_input.tmp
    timeout $timeout_duration bash < test_input.tmp > bash_stdout.tmp 2> bash_stderr.tmp
    local bash_exit=$?
    
    echo -e "${BLUE}STDOUT Comparison:${NC}"
    echo "Minishell: '$(cat mini_stdout_clean.tmp | tr '\n' '|')'"
    echo "Bash:      '$(cat bash_stdout.tmp | tr '\n' '|')'"
    
    echo -e "\n${BLUE}STDERR Comparison:${NC}"
    echo "Minishell: '$(cat mini_stderr.tmp | tr '\n' '|')'"
    echo "Bash:      '$(cat bash_stderr.tmp | tr '\n' '|')'"
    
    echo -e "\n${BLUE}EXIT CODE Comparison:${NC}"
    echo "Minishell: $mini_exit"
    echo "Bash:      $bash_exit"
    
    # Compare outputs
    local stdout_match=false
    local stderr_match=false
    local exit_match=false
    
    if cmp -s mini_stdout_clean.tmp bash_stdout.tmp; then
        stdout_match=true
        echo -e "${GREEN}✓ STDOUT matches${NC}"
    else
        echo -e "${RED}✗ STDOUT differs${NC}"
    fi
    
    if cmp -s mini_stderr.tmp bash_stderr.tmp; then
        stderr_match=true
        echo -e "${GREEN}✓ STDERR matches${NC}"
    else
        echo -e "${RED}✗ STDERR differs${NC}"
    fi
    
    if [ "$mini_exit" = "$bash_exit" ]; then
        exit_match=true
        echo -e "${GREEN}✓ EXIT CODE matches${NC}"
    else
        echo -e "${RED}✗ EXIT CODE differs${NC}"
    fi
    
    if [ "$stdout_match" = true ] && [ "$stderr_match" = true ] && [ "$exit_match" = true ]; then
        echo -e "${GREEN}🎉 PERFECT MATCH${NC}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}❌ BEHAVIOR DIFFERS${NC}"
        FAILED=$((FAILED + 1))
    fi
    
    rm -f test_input.tmp mini_stdout.tmp mini_stderr.tmp mini_stdout_clean.tmp bash_stdout.tmp bash_stderr.tmp
    echo "========================================"
}

# Test the three problematic scenarios
echo -e "${BLUE}Testing the three complex scenarios mentioned:${NC}"

compare_with_bash "Complex nested parentheses with pipes" "((echo 1) | (echo 2) | (echo 3 | (echo 4)))"

compare_with_bash "Logical operators with pipes" "echo 1 | (echo 2 || echo 3 && echo 4) || echo 5 | echo 6"

compare_with_bash "Pipe chain with parentheses and wc" "echo 1 | (grep 1) | cat | (wc -l)"

# Additional related scenarios to ensure robustness
echo -e "\n${BLUE}Additional related scenarios:${NC}"

compare_with_bash "Simple parentheses with pipe" "(echo hello) | cat"

compare_with_bash "Nested parentheses simple" "((echo test))"

compare_with_bash "Logical OR with pipes" "echo fail | grep nonexistent || echo success"

compare_with_bash "Logical AND with pipes" "echo test | grep test && echo found"

compare_with_bash "Mixed logical and pipes" "echo 1 | grep 1 && echo 2 | cat || echo 3"

# Summary
echo -e "\n${BLUE}=== FINAL SUMMARY ===${NC}"
echo "Total tests: $TOTAL"
echo -e "Perfect matches: ${GREEN}$PASSED${NC}"
echo -e "Behavior differs: ${RED}$FAILED${NC}"

if [ $FAILED -eq 0 ]; then
    echo -e "\n${GREEN}🎉 ALL SCENARIOS MATCH BASH EXACTLY!${NC}"
    exit 0
else
    echo -e "\n${RED}❌ SOME SCENARIOS NEED FIXES${NC}"
    echo "The scenarios that differ need to be investigated and fixed."
    exit 1
fi 