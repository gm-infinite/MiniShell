#!/bin/bash

# Advanced pipe and integration tests

echo "=== Advanced Pipe Tests ==="

timeout 15 bash << 'EOF'
cd /home/user/Desktop/minishell

echo "1. Testing pipes with logical operators: echo success | cat && echo next"
echo "echo success | cat && echo next" | ./minishell

echo -e "\n2. Testing pipes with logical operators failure: false | cat && echo should_not_print"
echo "false | cat && echo should_not_print" | ./minishell

echo -e "\n3. Testing pipes with OR: false | cat || echo backup"
echo "false | cat || echo backup" | ./minishell

echo -e "\n4. Testing more complex pipe chain: echo test | cat | wc -l"
echo "echo test | cat | wc -l" | ./minishell

echo -e "\n5. Testing pipe with multiple args: echo one two three | cat"
echo "echo one two three | cat" | ./minishell

echo -e "\n6. Testing pipe exit status: echo test | cat; echo \$?"
echo "echo test | cat; echo \$?" | ./minishell

echo -e "\n7. Exit"
echo "exit" | ./minishell

EOF

echo "=== Advanced Tests Complete ==="
