#!/bin/bash

echo "=== Edge Case Testing for Minishell ==="
echo "Testing complex edge cases for parentheses, logical operators, and memory management..."
echo

# Function to run a test with timeout and error checking
run_test() {
    local test_name="$1"
    local test_content="$2"
    echo "Test: $test_name"
    echo -e "$test_content\nexit" > temp_test.tmp
    timeout 10s ./minishell < temp_test.tmp
    local exit_code=$?
    if [ $exit_code -eq 124 ]; then
        echo "ERROR: Test timed out!"
    elif [ $exit_code -ne 0 ]; then
        echo "ERROR: Test exited with code $exit_code"
    fi
    echo "---"
}

echo "=== Deep Nesting Tests ==="
run_test "Triple nested parentheses" "(((echo deep)))"
run_test "Mixed nesting with operators" "(((echo a && echo b) || echo c) && echo d)"
run_test "Complex nested with pipes" "((echo hello | cat | cat) && echo world) || echo fail"

echo "=== Malformed Input Tests ==="
run_test "Unmatched opening parenthesis" "(echo test"
run_test "Unmatched closing parenthesis" "echo test)"
run_test "Multiple unmatched" "((echo test"
run_test "Empty command between operators" "echo start && && echo end"
run_test "Only operators" "&& ||"

echo "=== Stress Tests ==="
run_test "Many sequential operations" "echo 1 && echo 2 && echo 3 && echo 4 && echo 5"
run_test "Many sequential with parentheses" "(echo 1) && (echo 2) && (echo 3) && (echo 4)"
run_test "Deeply nested logical" "(echo a && (echo b || (echo c && echo d)))"

echo "=== Special Characters and Quoting ==="
run_test "Parentheses in quotes" "echo '(test)' && echo success"
run_test "Operators in quotes" "echo '&&' && echo '||'"
run_test "Mixed quotes and parentheses" "(echo \"hello world\") && echo 'success'"

echo "=== Environment Variable Edge Cases ==="
run_test "Complex env vars in parentheses" "(export A=1 && export B=2) && echo \$A\$B"
run_test "Unset variables" "(echo \$NONEXISTENT) && echo done"
run_test "Environment in nested contexts" "((export X=nested && echo \$X) && echo outer)"

echo "=== Redirection with Parentheses ==="
run_test "Redirection in parentheses" "(echo hello > /tmp/test.out) && cat /tmp/test.out"
run_test "Complex redirection" "(echo line1 && echo line2) > /tmp/test2.out && cat /tmp/test2.out"
run_test "Error redirection" "(ls /nonexistent 2>/dev/null || echo handled) && echo success"

echo "=== Exit Code Propagation ==="
run_test "False in parentheses" "(false) && echo should_not_see"
run_test "True in parentheses" "(true) && echo should_see"
run_test "Mixed exit codes" "(false || true) && echo result"
run_test "Exit code chain" "(exit 42) || echo caught"

echo "=== Memory Intensive Tests ==="
run_test "Long command line" "$(printf 'echo test && ' | head -c 500)echo final"
run_test "Many variables" "$(for i in {1..20}; do echo "export VAR$i=$i &&"; done | tr -d '\n') echo done"

echo "=== All Edge Case Tests Completed ==="
rm -f temp_test.tmp /tmp/test*.out
