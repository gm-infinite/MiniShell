#!/bin/bash

echo "=== QUICK MEMORY LEAK RETEST ==="

cd /home/user/Desktop/minishell

# Create a focused test for the specific leaky functions
cat > leak_retest_input.txt << 'EOF'
export TEST_VAR=test_value
echo $TEST_VAR
cd /tmp
echo $USER
unset TEST_VAR
exit
EOF

echo "🔍 Running focused valgrind test on fixed code..."

valgrind \
    --tool=memcheck \
    --leak-check=full \
    --show-leak-kinds=definite \
    --track-origins=yes \
    --log-file=leak_retest.log \
    ./minishell < leak_retest_input.txt

echo "✅ Test completed!"

# Check results
if [ -f leak_retest.log ]; then
    echo ""
    echo "📊 Memory leak results:"
    grep "definitely lost:" leak_retest.log || echo "No definite leaks found!"
    
    DEFINITELY_LOST=$(grep "definitely lost:" leak_retest.log | grep -o '[0-9,]\+ bytes' | head -1)
    if [ -n "$DEFINITELY_LOST" ] && [ "$DEFINITELY_LOST" != "0 bytes" ]; then
        echo "❌ STILL LEAKING: $DEFINITELY_LOST"
        echo ""
        echo "Leak details:"
        grep -A 5 "definitely lost" leak_retest.log
    else
        echo "🎉 SUCCESS: NO DEFINITE MEMORY LEAKS!"
    fi
    
    echo ""
    echo "📋 Summary:"
    grep "ERROR SUMMARY" leak_retest.log || echo "No error summary found"
else
    echo "❌ Valgrind log not found!"
fi

# Cleanup
rm -f leak_retest_input.txt leak_retest.log

echo ""
echo "=== LEAK RETEST COMPLETE ==="
