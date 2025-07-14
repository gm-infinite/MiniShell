#!/bin/bash

# Pipe functionality test script for minishell

echo "=== Testing Pipe Functionality ==="

# Start minishell and test various pipe scenarios
timeout 10 bash << 'EOF'
cd /home/user/Desktop/minishell

echo "1. Testing basic pipe: echo hello | cat"
echo "echo hello | cat" | ./minishell

echo -e "\n2. Testing multiple commands: ls -la | head -5"
echo "ls -la | head -5" | ./minishell

echo -e "\n3. Testing pipe with grep: ls | grep make"
echo "ls | grep make" | ./minishell

echo -e "\n4. Testing 3-command pipeline: echo hello | cat | cat"
echo "echo hello | cat | cat" | ./minishell

echo -e "\n5. Testing pipe with built-in: echo test | pwd"
echo "echo test | pwd" | ./minishell

echo -e "\n6. Testing env variable in pipe: echo \$USER | cat"
echo "echo \$USER | cat" | ./minishell

echo -e "\n7. Testing empty command handling: | cat"
echo "| cat" | ./minishell

echo -e "\n8. Testing command not found in pipe: nonexistent | cat"
echo "nonexistent | cat" | ./minishell

echo -e "\n9. Exit minishell"
echo "exit" | ./minishell

EOF

echo "=== Pipe Tests Complete ==="
