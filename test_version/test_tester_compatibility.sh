#!/bin/bash

# Test script for the tester-compatible minishell

echo "=== Testing Non-Interactive Mode ==="
echo "Testing basic commands..."
printf "echo Hello World\npwd\necho Done\nexit\n" | ./minishell

echo ""
echo "=== Testing Exit Command (should have no debug output) ==="
echo "exit" | ./minishell
echo "Exit test completed (no 'exit' message should appear above)"

echo ""
echo "=== Testing Complex Commands ==="
printf "export TEST=hello\necho \$TEST\ncd /tmp\npwd\nexit\n" | ./minishell

echo ""
echo "=== Interactive Mode Test (will timeout after 3 seconds) ==="
echo "Type commands like 'echo test' or 'pwd', or just wait for timeout..."
timeout 3 ./minishell || echo "(Interactive mode test completed)"
