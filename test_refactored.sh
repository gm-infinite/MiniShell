#!/bin/bash

# Test the refactored minishell
echo "=== Testing Refactored Minishell ==="

# Test builtin commands
echo "Testing builtin commands:"
echo "pwd" | timeout 3s ./minishell_refactored
echo "echo 'Hello World'" | timeout 3s ./minishell_refactored

# Test environment variables
echo "Testing environment variables:"
echo "export TEST_VAR=hello" | timeout 3s ./minishell_refactored
echo "echo \$USER" | timeout 3s ./minishell_refactored

echo "=== Test Complete ==="
