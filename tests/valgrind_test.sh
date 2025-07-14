#!/bin/bash

echo "=== COMPREHENSIVE VALGRIND MEMORY LEAK TEST ==="

cd /home/user/Desktop/minishell

# Build the project first
echo "Building minishell..."
make clean && make
if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

echo "✅ Build successful!"

# Create comprehensive test input file
cat > valgrind_test_input.txt << 'EOF'
pwd
echo hello world
echo $USER
echo $PWD
export TEST_VAR=test_value
echo $TEST_VAR
unset TEST_VAR
echo $TEST_VAR
env | head -3
cd /tmp
pwd
cd -
echo "single quotes test"
echo 'single $USER'
echo "double $USER quotes"
echo test | cat
echo hello | cat | cat
ls | head -2
echo test > output1.txt
cat < output1.txt
echo append >> output1.txt
cat output1.txt
echo line1 > input.txt
echo line2 >> input.txt
cat < input.txt | wc -l
true && echo success
false || echo backup
echo pipe1 | cat && echo logical
echo test | cat > pipe_output.txt
cat pipe_output.txt
rm -f output1.txt input.txt pipe_output.txt
echo 'complex: $USER' | cat > complex.txt && cat complex.txt
rm -f complex.txt
exit
EOF

echo "📝 Created comprehensive test input"

# Run valgrind with comprehensive options
echo "🔍 Running valgrind memory leak detection..."
echo "This may take a few minutes..."

valgrind \
    --tool=memcheck \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --suppressions=/dev/null \
    --log-file=valgrind_output.log \
    ./minishell < valgrind_test_input.txt

echo "✅ Valgrind test completed!"

# Analyze the results
echo ""
echo "=== VALGRIND RESULTS ANALYSIS ==="

if [ -f valgrind_output.log ]; then
    echo "📊 Memory leak summary:"
    grep -E "(definitely lost|indirectly lost|possibly lost|still reachable)" valgrind_output.log || echo "No leak summary found"
    
    echo ""
    echo "📋 Detailed analysis:"
    
    # Check for definitely lost (our code leaks)
    DEFINITELY_LOST=$(grep "definitely lost:" valgrind_output.log | grep -o '[0-9,]\+ bytes' | head -1)
    if [ -n "$DEFINITELY_LOST" ] && [ "$DEFINITELY_LOST" != "0 bytes" ]; then
        echo "❌ DEFINITELY LOST: $DEFINITELY_LOST"
        echo "   These are memory leaks in YOUR code that must be fixed!"
        echo ""
        echo "Details of definitely lost blocks:"
        grep -A 10 "definitely lost" valgrind_output.log
    else
        echo "✅ NO DEFINITELY LOST: No memory leaks in your code!"
    fi
    
    # Check for indirectly lost
    INDIRECTLY_LOST=$(grep "indirectly lost:" valgrind_output.log | grep -o '[0-9,]\+ bytes' | head -1)
    if [ -n "$INDIRECTLY_LOST" ] && [ "$INDIRECTLY_LOST" != "0 bytes" ]; then
        echo "⚠️  INDIRECTLY LOST: $INDIRECTLY_LOST"
        echo "   These are usually caused by definitely lost blocks"
    else
        echo "✅ NO INDIRECTLY LOST: Good!"
    fi
    
    # Check for possibly lost
    POSSIBLY_LOST=$(grep "possibly lost:" valgrind_output.log | grep -o '[0-9,]\+ bytes' | head -1)
    if [ -n "$POSSIBLY_LOST" ] && [ "$POSSIBLY_LOST" != "0 bytes" ]; then
        echo "⚠️  POSSIBLY LOST: $POSSIBLY_LOST"
        echo "   These might be from optimized data structures or readline"
    else
        echo "✅ NO POSSIBLY LOST: Excellent!"
    fi
    
    # Check for still reachable (usually readline and system libraries)
    STILL_REACHABLE=$(grep "still reachable:" valgrind_output.log | grep -o '[0-9,]\+ bytes' | head -1)
    if [ -n "$STILL_REACHABLE" ]; then
        echo "ℹ️  STILL REACHABLE: $STILL_REACHABLE"
        echo "   This is usually from readline() and system libraries (allowed)"
    fi
    
    echo ""
    echo "📄 Full valgrind log saved to: valgrind_output.log"
    echo "   Use 'cat valgrind_output.log' to see complete details"
    
    # Show any error summary
    echo ""
    echo "🔍 Error summary:"
    grep "ERROR SUMMARY" valgrind_output.log || echo "No error summary found"
    
else
    echo "❌ Valgrind output file not found!"
fi

# Cleanup
rm -f valgrind_test_input.txt output1.txt input.txt pipe_output.txt complex.txt

echo ""
echo "=== MEMORY LEAK TEST COMPLETE ==="
echo ""
echo "📋 What to look for:"
echo "✅ 'definitely lost: 0 bytes' - NO leaks in your code"
echo "⚠️  Any 'definitely lost' > 0 - YOUR code has leaks (must fix)"
echo "ℹ️  'still reachable' - Usually readline/system (allowed)"
echo "ℹ️  'possibly lost' - Might be readline optimizations (usually OK)"
