#!/bin/bash

echo "=== Testing Export vs Env Sorting ==="
echo

echo "1. Testing real bash export (should be sorted):"
export | head -3
echo

echo "2. Testing real bash env (should be unsorted):"
env | head -3
echo

echo "3. Testing our minishell export (should be sorted):"
cd /home/user/Desktop/minishell
printf "export\nexit\n" | ./minishell | grep -v "minishell >" | head -3
echo

echo "4. Testing our minishell env (should be unsorted):"
printf "env\nexit\n" | ./minishell | grep -v "minishell >" | head -3
echo

echo "=== Comparison Complete ==="
