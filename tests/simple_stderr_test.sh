#!/bin/bash

echo "=== Simple Stderr Test ==="
echo "Testing: ls /nonexistent 2> /tmp/simple_stderr.out"
echo -e "ls /nonexistent 2> /tmp/simple_stderr.out\nexit" > simple_test.tmp
./minishell < simple_test.tmp

echo "Checking if file was created:"
ls -la /tmp/simple_stderr.out 2>/dev/null && echo "File exists, content:" && cat /tmp/simple_stderr.out || echo "File not found"

echo ""
echo "Testing with space before >:"
echo -e "ls /nonexistent 2 > /tmp/simple_stderr2.out\nexit" > simple_test2.tmp  
./minishell < simple_test2.tmp

echo "Checking second file:"
ls -la /tmp/simple_stderr2.out 2>/dev/null && echo "File exists, content:" && cat /tmp/simple_stderr2.out || echo "File not found"

rm -f simple_test*.tmp /tmp/simple_stderr*.out
