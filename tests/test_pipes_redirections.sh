#!/bin/bash

echo "=== Testing Pipes with Redirections ==="

timeout 15 bash << 'EOF'
cd /home/user/Desktop/minishell

echo "1. Testing pipe with output redirection: echo hello | cat > output.txt"
echo "echo hello | cat > output.txt" | ./minishell
echo "Checking output.txt:"
cat output.txt 2>/dev/null || echo "File not found"

echo -e "\n2. Testing pipe with input redirection: echo test > input.txt; cat < input.txt | wc -l"
echo "echo test > input.txt" | ./minishell
echo "cat < input.txt | wc -l" | ./minishell

echo -e "\n3. Testing pipe with here document: cat << EOF | wc -l"
echo -e "cat << EOF | wc -l\nline1\nline2\nEOF" | ./minishell

echo -e "\n4. Testing complex: echo data | cat > temp.txt && cat temp.txt"
echo "echo data | cat > temp.txt && cat temp.txt" | ./minishell

echo -e "\n5. Testing multiple redirections in pipe: echo start | cat > out1.txt | cat > out2.txt"
echo "echo start | cat > out1.txt | cat > out2.txt" | ./minishell

echo -e "\n6. Exit"
echo "exit" | ./minishell

EOF

# Cleanup test files
rm -f output.txt input.txt temp.txt out1.txt out2.txt

echo "=== Pipes with Redirections Test Complete ==="
