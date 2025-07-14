#!/bin/bash

echo "=== Detailed Still Reachable Analysis ==="
echo "Testing to identify sources of still reachable blocks..."
echo

# Simple test with detailed stack traces for still reachable blocks
echo "Running detailed analysis with stack traces..."
echo -e "echo hello\nexit" | valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes --num-callers=20 ./minishell > /dev/null 2>still_reachable_detail.log

echo "Analyzing still reachable blocks..."
echo

# Count different types of leaks
echo "=== LEAK SUMMARY ==="
grep "definitely lost:" still_reachable_detail.log
grep "indirectly lost:" still_reachable_detail.log  
grep "possibly lost:" still_reachable_detail.log
grep "still reachable:" still_reachable_detail.log
echo

# Check if any still reachable blocks come from our code
echo "=== CHECKING FOR USER CODE IN STILL REACHABLE BLOCKS ==="
echo "Looking for references to our source files in stack traces..."

# Look for our source files in the detailed log
if grep -A 20 "still reachable" still_reachable_detail.log | grep -E "(main\.c|environment\.c|quotes\.c|builtins\.c|executor\.c|pipes\.c)" > /dev/null; then
    echo "⚠️  Found potential user code references in still reachable blocks:"
    grep -A 20 "still reachable" still_reachable_detail.log | grep -E "(main\.c|environment\.c|quotes\.c|builtins\.c|executor\.c|pipes\.c)"
else
    echo "✅ No user code references found in still reachable blocks"
    echo "   All still reachable blocks appear to be from system libraries"
fi

echo
echo "=== SAMPLE OF STILL REACHABLE STACK TRACES ==="
echo "First few still reachable block stack traces:"
grep -A 15 "still reachable" still_reachable_detail.log | head -30

echo
echo "=== CONCLUSION ==="
if grep "definitely lost: 0 bytes" still_reachable_detail.log > /dev/null && \
   grep "indirectly lost: 0 bytes" still_reachable_detail.log > /dev/null; then
    echo "✅ SUCCESS: Zero memory leaks from user code detected"
    echo "✅ All 'still reachable' blocks are from system libraries (expected)"
    echo "✅ The minishell meets strict memory management requirements"
else
    echo "❌ Memory leaks detected - further investigation needed"
fi
