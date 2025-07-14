#!/bin/bash

echo "=== Testing Stderr Redirection ==="

echo "Test 1: Basic stderr redirection"
echo -e "ls /nonexistent 2>/tmp/stderr_test.out && cat /tmp/stderr_test.out\nexit" > stderr_test1.tmp
./minishell < stderr_test1.tmp
echo "---"

echo "Test 2: Stderr redirection with success case" 
echo -e "ls /tmp 2>/tmp/stderr_test2.out && echo 'No errors redirected'\nexit" > stderr_test2.tmp
./minishell < stderr_test2.tmp
echo "---"

echo "Test 3: Combined stdout and stderr redirection"
echo -e "ls /tmp /nonexistent >/tmp/stdout.out 2>/tmp/stderr.out && echo 'Done' && cat /tmp/stdout.out && cat /tmp/stderr.out\nexit" > stderr_test3.tmp
./minishell < stderr_test3.tmp
echo "---"

# Cleanup
rm -f stderr_test*.tmp /tmp/*test*.out /tmp/stdout.out /tmp/stderr.out
echo "=== Stderr Tests Completed ==="
