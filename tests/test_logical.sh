#!/bin/bash

echo "=== Testing Logical Operators ==="

timeout 10 bash << 'EOF'
cd /home/user/Desktop/minishell

echo "1. Testing true && echo success"
echo "true && echo success" | ./minishell

echo -e "\n2. Testing false && echo should_not_print"
echo "false && echo should_not_print" | ./minishell

echo -e "\n3. Testing false || echo backup"
echo "false || echo backup" | ./minishell

echo -e "\n4. Testing true || echo should_not_print"
echo "true || echo should_not_print" | ./minishell

echo -e "\n5. Testing command with exit 0: echo test"
echo "echo test" | ./minishell

echo -e "\n6. Testing /usr/bin/false"
echo "/usr/bin/false" | ./minishell

echo -e "\n7. Exit"
echo "exit" | ./minishell

EOF
