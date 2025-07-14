#!/bin/bash

echo "Testing redirection detection..."

cd /home/user/Desktop/minishell

# Test if redirection works by testing in bash first
echo "Testing bash redirection:"
echo "hello from bash" > bash_test.txt
cat bash_test.txt
rm bash_test.txt

echo -e "\nTesting minishell redirection..."
echo "echo hello from minishell > minishell_test.txt" | ./minishell

echo "Checking if file was created:"
ls -la minishell_test.txt 2>/dev/null && echo "File exists!" && cat minishell_test.txt || echo "File not found"

# Cleanup
rm -f minishell_test.txt

echo "exit" | ./minishell
