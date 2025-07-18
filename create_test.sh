#!/bin/bash

echo "=== REFACTORED MINISHELL FUNCTIONALITY TEST ==="
echo ""

# Test commands
test_commands=(
    "echo 'Hello World!'"
    "pwd"
    "export TEST_VAR=hello"
    "echo \$TEST_VAR"
    "ls /usr/bin | head -3"
    "cat /etc/hostname"
    "echo 'Test1' > /tmp/test_output.txt"
    "cat < /tmp/test_output.txt"
    "echo 'Line 1' && echo 'Line 2'"
    "exit"
)

echo "Creating test script..."
cat > test_refactored.sh << 'EOF'
#!/bin/bash

# Test the refactored minishell
echo "=== Testing Refactored Minishell ==="

# Test builtin commands
echo "Testing builtin commands:"
echo "pwd" | timeout 3s ./minishell_refactored
echo "echo 'Hello World'" | timeout 3s ./minishell_refactored

# Test environment variables
echo "Testing environment variables:"
echo "export TEST_VAR=hello" | timeout 3s ./minishell_refactored
echo "echo \$USER" | timeout 3s ./minishell_refactored

echo "=== Test Complete ==="
EOF

chmod +x test_refactored.sh
echo "Test script created. You can run: ./test_refactored.sh"
