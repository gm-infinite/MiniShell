#!/bin/bash

echo "Testing cd with too many arguments..."

# Test 1: cd with two arguments (should fail)
echo "cd /tmp extra_arg" | /home/user/Desktop/minishell/minishell > /tmp/mini_out 2> /tmp/mini_err
mini_exit=$?
echo "cd /tmp extra_arg" | bash > /tmp/bash_out 2> /tmp/bash_err
bash_exit=$?

echo "Mini exit code: $mini_exit"
echo "Bash exit code: $bash_exit"
echo "Mini error: $(cat /tmp/mini_err)"
echo "Bash error: $(cat /tmp/bash_err)"

if [ "$mini_exit" = "$bash_exit" ]; then
    echo "✅ Exit codes match!"
else
    echo "❌ Exit codes don't match!"
fi

# Clean up
rm -f /tmp/mini_out /tmp/mini_err /tmp/bash_out /tmp/bash_err
