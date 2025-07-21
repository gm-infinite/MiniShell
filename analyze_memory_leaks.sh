#!/bin/bash

# Memory Leak Analysis Script
# This script identifies the exact sources of memory leaks in minishell

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== MINISHELL MEMORY LEAK ANALYSIS ===${NC}"
echo "Identifying exact leak sources..."
echo

# First, let's check a simple case that should not leak
echo -e "${YELLOW}1. Testing basic echo command:${NC}"
echo 'echo hello' | valgrind --leak-check=full --show-leak-kinds=all ./minishell 2>&1 | grep -E "(definitely|indirectly|possibly|still reachable)" | grep -v " 0 bytes"

echo -e "\n${YELLOW}2. Testing variable expansion:${NC}"
echo 'echo $USER' | valgrind --leak-check=full --show-leak-kinds=all ./minishell 2>&1 | grep -E "(definitely|indirectly|possibly|still reachable)" | grep -v " 0 bytes"

echo -e "\n${YELLOW}3. Testing pipe with variable:${NC}"
echo 'echo $USER | cat' | valgrind --leak-check=full --show-leak-kinds=all ./minishell 2>&1 | grep -E "(definitely|indirectly|possibly|still reachable)" | grep -v " 0 bytes"

echo -e "\n${YELLOW}4. Testing export command:${NC}"
echo -e 'export TEST=value\nexit' | valgrind --leak-check=full --show-leak-kinds=all ./minishell 2>&1 | grep -E "(definitely|indirectly|possibly|still reachable)" | grep -v " 0 bytes"

echo -e "\n${BLUE}=== IDENTIFIED MEMORY LEAK SOURCES ===${NC}"
echo
echo -e "${RED}PROBLEM 1: expand_variables_quoted() in main/env_expand.c${NC}"
echo "  - expand_tilde() allocates memory"
echo "  - If expand_variables() returns the same pointer (no expansion), tilde_expanded is not freed"
echo "  - Fix: Always free tilde_expanded if it's different from result"
echo
echo -e "${RED}PROBLEM 2: process_and_check_args() in exec_arg_utils.c${NC}"
echo "  - Overwrites args[i] without freeing the old value"
echo "  - Each expand_variables_quoted() call allocates new memory"
echo "  - Fix: Store old pointer, assign new, then free old"
echo
echo -e "${RED}PROBLEM 3: Similar issues in other expansion functions${NC}"
echo "  - process_variable_expansion() in redir_args.c"
echo "  - process_argument_expansion() in execute_pipe_child.c"
echo "  - validate_and_process_args() in exec_validation.c"
echo
echo -e "${GREEN}=== RECOMMENDED FIXES ===${NC}"
echo "1. Fix expand_variables_quoted to always free intermediate allocations"
echo "2. Fix all functions that overwrite pointers to free old values first"
echo "3. Add proper cleanup in error paths"
echo "4. Ensure consistent memory ownership rules" 