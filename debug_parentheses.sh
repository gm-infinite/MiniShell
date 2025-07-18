#!/bin/bash

# Simple test to understand the tokenization
cd /home/user/Desktop/minishell

echo "Testing simple tokenization patterns:"

echo "1. Testing single parentheses:"
echo "(echo test)" | ./minishell

echo "2. Testing pipe without parentheses:"
echo "echo first | echo second" | ./minishell

echo "3. Testing the problematic case:"
echo "(echo first) | (echo second)" | ./minishell

echo "4. Testing nested case that works:"
echo "(echo first | echo second)" | ./minishell
