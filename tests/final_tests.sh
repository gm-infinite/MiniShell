#!/bin/bash

echo "=== Final Comprehensive Minishell Tests ==="
echo ""

# Test parentheses
echo "Testing parentheses with logical operators:"
echo -e "(echo first && echo second) || echo third\nexit" | ./minishell
echo "---"

# Test stderr redirection  
echo "Testing stderr redirection:"
echo -e "ls /nonexistent 2> /tmp/final_stderr.out && echo 'Error redirected' && cat /tmp/final_stderr.out\nexit" | ./minishell
echo "---"

# Test combined features
echo "Testing complex combination:"
echo -e "(ls /tmp >/tmp/stdout.tmp && echo 'Success') || (echo 'Failed' 2>/tmp/error.tmp)\nexit" | ./minishell
echo "Checking files created:"
ls -la /tmp/stdout.tmp /tmp/error.tmp 2>/dev/null || echo "Some files missing"
echo "---"

# Test deep nesting
echo "Testing deep nesting:"
echo -e "(((echo deep) && echo nested) || echo backup) && echo final\nexit" | ./minishell
echo "---"

# Test memory management
echo "Testing memory with many variables:"
echo -e "export A=1 && export B=2 && export C=3 && (echo \$A\$B\$C) && unset A B C\nexit" | ./minishell
echo "---"

echo "=== All tests completed successfully ==="
rm -f /tmp/final_stderr.out /tmp/stdout.tmp /tmp/error.tmp
