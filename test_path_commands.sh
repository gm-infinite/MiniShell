#!/bin/bash

# Test script for broken path commands
# This compares minishell behavior with bash exactly

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== PATH COMMANDS COMPARISON TEST ===${NC}"
echo

# Function to compare minishell and bash behavior exactly
compare_path_command() {
    local test_name="$1"
    local test_cmd="$2"
    
    echo -e "\n${YELLOW}Testing: $test_name${NC}"
    echo "Command: $test_cmd"
    echo "----------------------------------------"
    
    # Run with minishell
    echo -e "$test_cmd\nexit" > test_input.tmp
    timeout 5 ./minishell < test_input.tmp > mini_stdout.tmp 2> mini_stderr.tmp
    local mini_exit=$?
    
    # Clean minishell output (remove prompt)
    cat mini_stdout.tmp | grep -v "minishell >" > mini_stdout_clean.tmp
    
    # Run with bash
    echo -e "$test_cmd\nexit" > test_input.tmp
    timeout 5 bash < test_input.tmp > bash_stdout.tmp 2> bash_stderr.tmp
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
    local match=true
    
    if ! cmp -s mini_stdout_clean.tmp bash_stdout.tmp; then
        echo -e "${RED}✗ STDOUT differs${NC}"
        match=false
    else
        echo -e "${GREEN}✓ STDOUT matches${NC}"
    fi
    
    if ! cmp -s mini_stderr.tmp bash_stderr.tmp; then
        echo -e "${RED}✗ STDERR differs${NC}"
        match=false
    else
        echo -e "${GREEN}✓ STDERR matches${NC}"
    fi
    
    if [ "$mini_exit" != "$bash_exit" ]; then
        echo -e "${RED}✗ EXIT CODE differs${NC}"
        match=false
    else
        echo -e "${GREEN}✓ EXIT CODE matches${NC}"
    fi
    
    if [ "$match" = true ]; then
        echo -e "${GREEN}🎉 PERFECT MATCH${NC}"
    else
        echo -e "${RED}❌ BEHAVIOR DIFFERS${NC}"
    fi
    
    rm -f test_input.tmp mini_stdout.tmp mini_stderr.tmp mini_stdout_clean.tmp bash_stdout.tmp bash_stderr.tmp
    echo "========================================"
}

# Test all the broken path commands
compare_path_command "Absolute directory path" "/bin/"

compare_path_command "Non-existent absolute path" "/ls"

compare_path_command "Complex quoted path 2" '"./""ls"'

compare_path_command "Simple quoted path" '"./"ls'

compare_path_command "Relative path" "./ls"

echo -e "\n${BLUE}=== TEST COMPLETED ===${NC}" 