#!/bin/bash

# ************************************************************************** #
#                                                                            #
#                                                        :::      ::::::::   #
#   test_wildcard_fixed.sh                            :+:      :+:    :+:   #
#                                                    +:+ +:+         +:+     #
#   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        #
#                                                +#+#+#+#+#+   +#+           #
#   Created: 2025/07/14 00:00:00 by emgenc            #+#    #+#             #
#   Updated: 2025/07/14 00:00:00 by emgenc           ###   ########.fr       #
#                                                                            #
# ************************************************************************** #

# Fixed Wildcard Implementation Tester
# Tests wildcard functionality with proper command structure

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Global test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Paths
MINISHELL_PATH="./minishell"

# Function to print colored output
print_color() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

# Function to print test header
print_test_header() {
    local test_name=$1
    echo ""
    print_color "$CYAN" "==================== $test_name ===================="
}

# Function to run a test and check the output
run_test() {
    local test_name="$1"
    local command="$2"
    local expected_pattern="$3"  # Can be files to check for or "NONEMPTY" or "EMPTY"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    echo ""
    print_color "$YELLOW" "Test $TOTAL_TESTS: $test_name"
    echo "Command: $command"
    
    # Run the command through minishell
    local output
    output=$(echo "$command" | timeout 5s "$MINISHELL_PATH" 2>&1 | grep -v "minishell >" | sed '/^$/d' | tr '\n' ' ' | sed 's/[[:space:]]*$//')
    
    echo "Output: '$output'"
    
    # Check result based on expected pattern
    local test_passed=false
    
    case "$expected_pattern" in
        "NONEMPTY")
            if [ -n "$output" ] && ! echo "$output" | grep -q "command not found"; then
                test_passed=true
            fi
            ;;
        "EMPTY")
            if [ -z "$output" ] || echo "$output" | grep -q "command not found"; then
                test_passed=true
            fi
            ;;
        *)
            # Check if specific files are in output
            if [ -n "$output" ] && ! echo "$output" | grep -q "command not found"; then
                test_passed=true
                # Verify specific files if provided
                for file in $expected_pattern; do
                    if ! echo "$output" | grep -q "$file"; then
                        test_passed=false
                        echo "  Missing expected file: $file"
                        break
                    fi
                done
            fi
            ;;
    esac
    
    if [ "$test_passed" = true ]; then
        print_color "$GREEN" "  ✓ PASS"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        print_color "$RED" "  ✗ FAIL"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

# Test basic wildcard functionality
test_basic_wildcards() {
    print_test_header "Basic Wildcard Tests"
    
    # Create test files for these tests
    touch test_file1.txt test_file2.txt test_file3.log
    touch .hidden_file
    
    run_test "Simple asterisk pattern" "echo *.c" "test1.c test2.c"
    run_test "Pattern with extension" "echo *.txt" "test_file1.txt test_file2.txt"
    run_test "Pattern with specific extension" "echo *.log" "test_file3.log"
    run_test "Question mark pattern" "echo test?.c" "test1.c test2.c"
    run_test "Pattern that matches directories" "echo */.*" "NONEMPTY"
    
    # Clean up test files
    rm -f test_file1.txt test_file2.txt test_file3.log .hidden_file
}

# Test edge cases
test_edge_cases() {
    print_test_header "Edge Case Tests"
    
    run_test "No matches pattern" "echo *.xyz" "*.xyz"
    run_test "Multiple asterisks" "echo *.*" "NONEMPTY"
    run_test "Pattern at start" "echo test*" "test1.c test2.c"
    run_test "Pattern at end" "echo *.c" "test1.c test2.c"
}

# Test quoted patterns (should not expand)
test_quoted_patterns() {
    print_test_header "Quoted Pattern Tests"
    
    run_test "Single quoted pattern" "echo '*.c'" "*.c"
    run_test "Double quoted pattern" 'echo "*.c"' "*.c"
    run_test "Mixed quotes" "echo '*.c' *.txt" "NONEMPTY"
}

# Test with different commands
test_with_commands() {
    print_test_header "Command Integration Tests"
    
    touch cmd_test1.txt cmd_test2.txt
    
    run_test "ls with wildcard" "ls *.c" "test1.c test2.c"
    run_test "cat with wildcard (should try to open files)" "cat *.txt" "NONEMPTY"
    run_test "wc with wildcard" "wc *.c" "NONEMPTY"
    
    rm -f cmd_test1.txt cmd_test2.txt
}

# Test performance with many files
test_performance() {
    print_test_header "Performance Tests"
    
    # Create many files
    for i in {1..20}; do
        touch "perf_test_$i.txt"
    done
    
    run_test "Many files pattern" "echo perf_test_*.txt" "NONEMPTY"
    run_test "Pattern with large result" "echo *.txt *.c *.md" "NONEMPTY"
    
    # Clean up
    rm -f perf_test_*.txt
}

# Test current directory limitation
test_current_directory_only() {
    print_test_header "Current Directory Limitation Tests"
    
    # These should not work as they go outside current directory
    run_test "Parent directory pattern" "echo ../*.c" "../*.c"
    run_test "Subdirectory pattern" "echo main/*.c" "main/*.c"
    run_test "Absolute path pattern" "echo /tmp/*.txt" "/tmp/*.txt"
}

# Test memory and error conditions
test_error_conditions() {
    print_test_header "Error Condition Tests"
    
    run_test "Very long pattern" "echo $(printf '*.%0100d' 1)" "NONEMPTY"
    run_test "Pattern with special chars" "echo *[*.c" "NONEMPTY"
    run_test "Empty echo with pattern" "echo" "EMPTY"
}

# Main test runner
main() {
    print_color "$PURPLE" "========================================"
    print_color "$PURPLE" "   FIXED WILDCARD COMPREHENSIVE TESTS"
    print_color "$PURPLE" "========================================"
    
    # Start in minishell root directory (where the binary is)
    cd "$(dirname "$0")/.."
    
    # Check if minishell exists
    if [ ! -f "$MINISHELL_PATH" ]; then
        print_color "$RED" "Error: Minishell binary not found at $MINISHELL_PATH"
        print_color "$YELLOW" "Please run 'make' in the minishell directory first"
        print_color "$YELLOW" "Current directory: $(pwd)"
        print_color "$YELLOW" "Looking for: $MINISHELL_PATH"
        exit 1
    fi
    
    # Run test suites
    test_basic_wildcards
    test_edge_cases
    test_quoted_patterns
    test_with_commands
    test_performance
    test_current_directory_only
    test_error_conditions
    
    # Print summary
    echo ""
    print_color "$PURPLE" "========================================"
    print_color "$PURPLE" "            TEST SUMMARY"
    print_color "$PURPLE" "========================================"
    echo "Total tests: $TOTAL_TESTS"
    print_color "$GREEN" "Passed: $PASSED_TESTS"
    print_color "$RED" "Failed: $FAILED_TESTS"
    
    if [ $FAILED_TESTS -eq 0 ]; then
        print_color "$GREEN" "🎉 ALL TESTS PASSED! 🎉"
        exit 0
    else
        local success_rate=$((PASSED_TESTS * 100 / TOTAL_TESTS))
        print_color "$YELLOW" "Success rate: $success_rate%"
        print_color "$RED" "Some tests failed. Check the output above for details."
        exit 1
    fi
}

# Run the tests
main "$@"
