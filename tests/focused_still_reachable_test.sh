#!/bin/bash

echo "=== FOCUSED STILL REACHABLE FIX ==="

cd /home/user/Desktop/minishell

# Create a simple test input
echo "echo hello" > simple_test.txt
echo "exit" >> simple_test.txt

echo "🔍 Running focused valgrind test..."

timeout 30 valgrind \
    --tool=memcheck \
    --leak-check=full \
    --show-reachable=yes \
    --track-origins=yes \
    --log-file=focused_still_reachable.log \
    ./minishell < simple_test.txt

if [ $? -eq 124 ]; then
    echo "⚠️ Test timed out after 30 seconds"
fi

echo "✅ Test completed!"

if [ -f focused_still_reachable.log ]; then
    echo ""
    echo "📊 Looking for still reachable blocks from our code (0x10xxxx)..."
    
    # Extract only the still reachable blocks that involve our code
    awk '/still reachable.*loss record/ {
        record = $0
        getline; stack1 = $0
        while (getline && $0 !~ /^==[0-9]/) {
            if ($0 ~ /0x10.*minishell/) {
                print record
                print stack1
                for(i=1; i<=5; i++) {
                    if ((getline) > 0) print
                    else break
                }
                print "---"
                break
            }
        }
    }' focused_still_reachable.log
    
    echo ""
    echo "📈 Summary:"
    grep "still reachable:" focused_still_reachable.log || echo "No summary found"
else
    echo "❌ Log file not found"
fi

# Cleanup
rm -f simple_test.txt focused_still_reachable.log

echo ""
echo "=== ANALYSIS COMPLETE ==="
