#!/bin/bash

# Test script to verify memory leak fixes

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== TESTING MEMORY LEAK FIXES ===${NC}"
echo "Running targeted tests on the fixed code..."
echo

# First, recompile the minishell
echo -e "${YELLOW}Recompiling minishell...${NC}"
make re

if [ ! -f "./minishell" ]; then
    echo -e "${RED}Failed to compile minishell!${NC}"
    exit 1
fi

echo -e "\n${GREEN}Compilation successful!${NC}"

# Function to run a single test
run_test() {
    local test_name="$1"
    local test_cmd="$2"
    
    echo -n -e "${YELLOW}Testing: $test_name${NC} ... "
    
    # Run with valgrind and capture output
    echo -e "$test_cmd\nexit" | valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes 2>&1 | grep -E "(definitely lost:|indirectly lost:|possibly lost:|still reachable:)" > tmp_leak_test.log
    
    # Check results
    local definitely_lost=$(grep "definitely lost:" tmp_leak_test.log | awk '{print $4}')
    local indirectly_lost=$(grep "indirectly lost:" tmp_leak_test.log | awk '{print $4}')
    local possibly_lost=$(grep "possibly lost:" tmp_leak_test.log | awk '{print $4}')
    local still_reachable=$(grep "still reachable:" tmp_leak_test.log | awk '{print $4}')
    
    # Remove commas
    definitely_lost=${definitely_lost//,/}
    indirectly_lost=${indirectly_lost//,/}
    possibly_lost=${possibly_lost//,/}
    still_reachable=${still_reachable//,/}
    
    # Check if we have any of our own leaks (not readline)
    if [[ "$definitely_lost" == "0" && "$indirectly_lost" == "0" && "$possibly_lost" == "0" ]]; then
        echo -e "${GREEN}PASS${NC} - No leaks from our code!"
    else
        echo -e "${RED}FAIL${NC}"
        echo "  Definitely lost: $definitely_lost bytes"
        echo "  Indirectly lost: $indirectly_lost bytes"
        echo "  Possibly lost: $possibly_lost bytes"
        cat tmp_leak_test.log
    fi
    
    rm -f tmp_leak_test.log
}

# Test cases that previously leaked
echo -e "\n${BLUE}=== TESTING FIXED LEAK SCENARIOS ===${NC}"

run_test "Simple variable expansion" 'echo $USER'
run_test "Variable in pipe" 'echo $USER | cat'
run_test "Multiple variables" 'echo $HOME $USER $PATH'
run_test "Export and use" 'export TEST=hello && echo $TEST'
run_test "Variable in quotes" 'echo "Hello $USER"'
run_test "Tilde expansion" 'echo ~'
run_test "Complex pipe with vars" 'echo $USER | grep $USER | cat'
run_test "Redirection with var" 'echo $USER > /tmp/test_file'
run_test "Here doc with var" 'cat << EOF
$USER
$HOME
EOF'

echo -e "\n${BLUE}=== EDGE CASES ===${NC}"

run_test "Empty variable" 'echo $NONEXISTENT'
run_test "Exit status var" 'echo $?'
run_test "Variable splitting" 'export X="a b c" && echo $X'
run_test "Nested quotes with var" 'echo "test '\''$USER'\'' test"'

# Cleanup
rm -f /tmp/test_file

echo -e "\n${GREEN}=== ALL TESTS COMPLETE ===${NC}"
echo "If all tests passed, the memory leaks have been successfully fixed!" 