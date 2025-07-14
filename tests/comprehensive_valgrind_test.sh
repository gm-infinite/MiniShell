#!/bin/bash

echo "=== Comprehensive Valgrind Memory Leak Test ==="
echo "Testing various shell operations for memory leaks..."
echo

# Test 1: Basic commands with variable expansion
echo "Test 1: Basic commands with variable expansion"
echo -e "echo \$USER\nexit" | valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=/dev/null ./minishell 2>&1 | grep -E "(definitely lost|indirectly lost|possibly lost|still reachable|ERROR|Invalid)" 
echo

# Test 2: Environment variables
echo "Test 2: Environment variables (export/unset)"
echo -e "export TEST_VAR=hello\necho \$TEST_VAR\nunset TEST_VAR\nexit" | valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./minishell 2>&1 | grep -E "(definitely lost|indirectly lost|possibly lost|still reachable|ERROR|Invalid)"
echo

# Test 3: Quotes and variable expansion
echo "Test 3: Quotes and variable expansion"
echo -e "echo \"Hello \$USER\"\necho 'No expansion \$USER'\nexit" | valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./minishell 2>&1 | grep -E "(definitely lost|indirectly lost|possibly lost|still reachable|ERROR|Invalid)"
echo

# Test 4: Built-in commands
echo "Test 4: Built-in commands"
echo -e "pwd\ncd /tmp\npwd\ncd -\nexit" | valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./minishell 2>&1 | grep -E "(definitely lost|indirectly lost|possibly lost|still reachable|ERROR|Invalid)"
echo

# Test 5: Simple pipes
echo "Test 5: Simple pipes"
echo -e "echo hello | cat\nexit" | valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./minishell 2>&1 | grep -E "(definitely lost|indirectly lost|possibly lost|still reachable|ERROR|Invalid)"
echo

# Test 6: Exit status
echo "Test 6: Exit status"
echo -e "false\necho \$?\nexit" | valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./minishell 2>&1 | grep -E "(definitely lost|indirectly lost|possibly lost|still reachable|ERROR|Invalid)"
echo

echo "=== Summary ==="
echo "All tests completed. Check above for any 'definitely lost' or 'indirectly lost' leaks."
echo "Note: 'still reachable' blocks from readline/system libraries are expected."
