#!/bin/bash

echo "=== TARGETED STILL REACHABLE ANALYSIS ==="

cd /home/user/Desktop/minishell

# Create a simple test that might trigger the still reachable issues
cat > still_reachable_test.txt << 'EOF'
echo hello
echo $USER  
ls | head -1
exit
EOF

echo "🔍 Running targeted valgrind to analyze still reachable from our code..."

valgrind \
    --tool=memcheck \
    --leak-check=full \
    --show-reachable=yes \
    --track-origins=yes \
    --log-file=still_reachable.log \
    ./minishell < still_reachable_test.txt

echo "✅ Analysis complete!"

if [ -f still_reachable.log ]; then
    echo ""
    echo "📊 Searching for still reachable blocks from our code..."
    
    # Look for still reachable blocks that involve our functions
    echo "--- Blocks involving expand_variables_quoted ---"
    grep -A 10 -B 2 "expand_variables_quoted" still_reachable.log || echo "None found"
    
    echo ""
    echo "--- Blocks involving execute_pipeline ---"
    grep -A 10 -B 2 "execute_pipeline" still_reachable.log || echo "None found"
    
    echo ""
    echo "--- Blocks involving our minishell functions (0x10) ---"
    grep -A 8 "by 0x10" still_reachable.log | grep -B 3 -A 5 "still reachable" | head -20
    
    echo ""
    echo "📈 Summary of still reachable memory:"
    grep "still reachable:" still_reachable.log
    
    echo ""
    echo "🔍 Breakdown by source:"
    echo "Our code (0x10xxxx addresses):"
    grep -c "by 0x10.*minishell" still_reachable.log || echo "0"
    echo "Readline library:"
    grep -c "libreadline" still_reachable.log || echo "0"
    echo "System libraries:"
    grep -c "lib.*\.so" still_reachable.log || echo "0"
else
    echo "❌ Valgrind log not found!"
fi

# Cleanup
rm -f still_reachable_test.txt still_reachable.log

echo ""
echo "=== ANALYSIS COMPLETE ==="
