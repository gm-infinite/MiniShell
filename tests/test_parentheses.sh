#!/bin/bash

echo "=== Comprehensive Parentheses Parser Testing ==="
echo "Testing parentheses handling with logical operators..."
echo

# Create temporary input files for each test to avoid printf issues
echo "Test 1: Basic parentheses with logical operators"
echo -e "(echo first && echo second) || echo third\nexit" > test1.tmp
timeout 5s ./minishell < test1.tmp
echo "---"

echo "Test 2: Nested logical operations"
echo -e "(true && echo success) || (false && echo fail)\nexit" > test2.tmp
timeout 5s ./minishell < test2.tmp
echo "---"

echo "Test 3: Multiple levels of parentheses"  
echo -e "((echo inner) && echo outer) || echo fallback\nexit" > test3.tmp
timeout 5s ./minishell < test3.tmp
echo "---"

echo "Test 4: Parentheses with exit codes"
echo -e "(false || echo rescued) && echo final\nexit" > test4.tmp
timeout 5s ./minishell < test4.tmp
echo "---"

echo "Test 5: Complex mixed operations"
echo -e "echo start && (echo mid1 || echo mid2) && echo end\nexit" > test5.tmp
timeout 5s ./minishell < test5.tmp
echo "---"

echo "Test 6: Parentheses with pipes"
echo -e "(echo hello | cat) && echo world\nexit" > test6.tmp
timeout 5s ./minishell < test6.tmp
echo "---"

echo "Test 7: Empty parentheses"
echo -e "() && echo after_empty\nexit" > test7.tmp
timeout 5s ./minishell < test7.tmp
echo "---"

echo "Test 8: Parentheses with environment variables"
echo -e "export TEST=hello && (echo \$TEST) || echo fail\nexit" > test8.tmp
timeout 5s ./minishell < test8.tmp
echo "---"

echo "Test 9: Complex real-world example"
echo -e "(export VAR=test && echo \$VAR) && (pwd | cat) || echo failed\nexit" > test9.tmp
timeout 5s ./minishell < test9.tmp
echo "---"

echo "=== All Parentheses Tests Completed ==="

# Cleanup
rm -f test*.tmp
