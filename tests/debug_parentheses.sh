#!/bin/bash

echo "=== Simple Parentheses Debugging ==="

# Test without parentheses first
echo "Test 1: Without parentheses"
timeout 3s bash -c 'printf "echo hello\nexit\n" | ./minishell'
echo "Exit code: $?"
echo "---"

# Test with logical operators
echo "Test 2: Logical operators"
timeout 3s bash -c 'printf "echo first && echo second\nexit\n" | ./minishell'
echo "Exit code: $?"
echo "---"

# Test with simple parentheses - this is where it might hang
echo "Test 3: Simple parentheses (potential hang)"
timeout 3s bash -c 'printf "(echo hello)\nexit\n" | ./minishell'
echo "Exit code: $?"
echo "---"

echo "Done"
