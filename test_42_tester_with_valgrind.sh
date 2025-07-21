#!/bin/bash

# Run 42_minishell_tester test cases with valgrind
# This helps identify which specific test cases are causing memory leaks

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

TESTER_DIR="42_minishell_tester"
VALGRIND_CMD="valgrind --leak-check=full --errors-for-leak-kinds=all --show-leak-kinds=all --track-origins=yes"
MINISHELL="./minishell"
LOG_DIR="valgrind_test_logs"

# Create log directory
mkdir -p "$LOG_DIR"

echo -e "${BLUE}=== RUNNING 42_MINISHELL_TESTER WITH VALGRIND ===${NC}"
echo "This will take some time as valgrind slows down execution..."
echo

# Function to extract and run individual tests from a test file
run_test_file_with_valgrind() {
    local test_file="$1"
    local test_category=$(basename "$test_file" .sh)
    local log_prefix="$LOG_DIR/${test_category}"
    
    echo -e "\n${YELLOW}Processing: $test_file${NC}"
    
    # Read the test file and extract individual tests
    local test_num=0
    local in_test=false
    local test_content=""
    local line_num=0
    
    while IFS= read -r line; do
        ((line_num++))
        
        # Skip comments and empty lines at the beginning
        if [[ "$line" =~ ^#.*$ ]] || [[ -z "$line" ]]; then
            if [[ "$in_test" == false ]]; then
                continue
            fi
        fi
        
        # If we hit a comment or empty line while in a test, execute the test
        if [[ "$in_test" == true ]] && ([[ "$line" =~ ^#.*$ ]] || [[ -z "$line" ]]); then
            if [[ -n "$test_content" ]]; then
                ((test_num++))
                local log_file="${log_prefix}_test_${test_num}_line_${line_num}.log"
                
                echo -n "  Test #$test_num (line $line_num): "
                
                # Run the test with valgrind
                echo -e "$test_content" | $VALGRIND_CMD $MINISHELL > /dev/null 2> "$log_file"
                
                # Quick check for leaks
                if grep -q "All heap blocks were freed" "$log_file"; then
                    echo -e "${GREEN}NO LEAKS${NC}"
                    rm "$log_file"  # Remove log if no leaks
                else
                    local definitely_lost=$(grep "definitely lost:" "$log_file" | awk '{print $4}')
                    local still_reachable=$(grep "still reachable:" "$log_file" | awk '{print $4}')
                    echo -e "${RED}LEAKS FOUND${NC} (definitely lost: $definitely_lost, still reachable: $still_reachable)"
                    echo "    Command: $(echo "$test_content" | head -n1 | tr '\n' ' ')"
                    echo "    Log: $log_file"
                fi
                
                test_content=""
                in_test=false
            fi
        else
            # Add line to current test
            if [[ -n "$line" ]] && ! [[ "$line" =~ ^#.*$ ]]; then
                in_test=true
                test_content+="$line"$'\n'
            fi
        fi
    done < "$test_file"
    
    # Handle last test if file doesn't end with empty line
    if [[ "$in_test" == true ]] && [[ -n "$test_content" ]]; then
        ((test_num++))
        local log_file="${log_prefix}_test_${test_num}_line_${line_num}.log"
        
        echo -n "  Test #$test_num (line $line_num): "
        
        echo -e "$test_content" | $VALGRIND_CMD $MINISHELL > /dev/null 2> "$log_file"
        
        if grep -q "All heap blocks were freed" "$log_file"; then
            echo -e "${GREEN}NO LEAKS${NC}"
            rm "$log_file"
        else
            local definitely_lost=$(grep "definitely lost:" "$log_file" | awk '{print $4}')
            local still_reachable=$(grep "still reachable:" "$log_file" | awk '{print $4}')
            echo -e "${RED}LEAKS FOUND${NC} (definitely lost: $definitely_lost, still reachable: $still_reachable)"
            echo "    Command: $(echo "$test_content" | head -n1 | tr '\n' ' ')"
            echo "    Log: $log_file"
        fi
    fi
}

# Run tests for each category
echo -e "${BLUE}MANDATORY TESTS:${NC}"

# Process each test file
for test_file in $TESTER_DIR/cmds/mand/*.sh; do
    if [[ -f "$test_file" ]]; then
        run_test_file_with_valgrind "$test_file"
    fi
done

echo -e "\n${BLUE}BONUS TESTS:${NC}"

for test_file in $TESTER_DIR/cmds/bonus/*.sh; do
    if [[ -f "$test_file" ]]; then
        run_test_file_with_valgrind "$test_file"
    fi
done

# Summary
echo -e "\n${BLUE}=== SUMMARY ===${NC}"
echo "Valgrind logs with memory leaks saved in: $LOG_DIR/"
echo "Logs without leaks were automatically removed."
echo
echo "To analyze a specific leak, run:"
echo "  cat $LOG_DIR/<log_file>"
echo
echo "To find all files with leaks:"
echo "  ls -la $LOG_DIR/*.log 2>/dev/null | wc -l"

# Show leak statistics
leak_count=$(ls -la $LOG_DIR/*.log 2>/dev/null | wc -l)
if [[ $leak_count -eq 0 ]]; then
    echo -e "\n${GREEN}Excellent! No memory leaks detected in any test.${NC}"
else
    echo -e "\n${RED}Found $leak_count test cases with memory leaks.${NC}"
    echo "Most common leak patterns:"
    grep -h "definitely lost:" $LOG_DIR/*.log 2>/dev/null | sort | uniq -c | sort -nr | head -5
fi

echo -e "\n${GREEN}Testing complete!${NC}" 