#!/bin/bash

echo "Testing exit with too many arguments..."

# Test exit with two arguments (should fail)
echo "exit 42 world" | /home/user/Desktop/minishell/minishell > /tmp/mini_out 2> /tmp/mini_err
mini_exit=$?
echo "exit 42 world" | bash > /tmp/bash_out 2> /tmp/bash_err
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
