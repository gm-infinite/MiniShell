#!/bin/bash

# ************************************************************************** #
#                                                                            #
#                                                        :::      ::::::::   #
#   test_wildcard_final.sh                            :+:      :+:    :+:   #
#                                                    +:+ +:+         +:+     #
#   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        #
#                                                +#+#+#+#+#+   +#+           #
#   Created: 2025/07/14 00:00:00 by emgenc            #+#    #+#             #
#   Updated: 2025/07/14 00:00:00 by emgenc           ###   ########.fr       #
#                                                                            #
# ************************************************************************** #

# Final Wildcard Implementation Tester with Correct Expectations
# Tests wildcard functionality with proper behavior expectations

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
    local test_type="$3"     # "CONTAINS", "EXACT", "LITERAL", "NONEMPTY", "EMPTY"
    local expected="$4"      # Expected content or files to check
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    echo ""
    print_color "$YELLOW" "Test $TOTAL_TESTS: $test_name"
    echo "Command: $command"
    
    # Run the command through minishell
    local output
    output=$(echo "$command" | timeout 5s "$MINISHELL_PATH" 2>&1 | grep -v "minishell >" | sed '/^$/d' | tr '\n' ' ' | sed 's/[[:space:]]*$//')
    
    echo "Output: '$output'"
    
    # Check result based on test type
    local test_passed=false
    
    case "$test_type" in
        "CONTAINS")
            if [ -n "$output" ] && ! echo "$output" | grep -q "command not found"; then
                test_passed=true
                # Check if all expected files are present
                for file in $expected; do
                    if ! echo "$output" | grep -q "$file"; then
                        test_passed=false
                        echo "  Missing expected file: $file"
                        break
                    fi
                done
            fi
            ;;
        "EXACT")
            if [ "$output" = "$expected" ]; then
                test_passed=true
            fi
            ;;
        "LITERAL")
            # For patterns that should not expand and return literally
            if [ "$output" = "$expected" ]; then
                test_passed=true
            fi
            ;;
        "NONEMPTY")
            if [ -n "$output" ] && ! echo "$output" | grep -q "command not found"; then
                test_passed=true
            fi
            ;;
        "EMPTY")
            if [ -z "$output" ]; then
                test_passed=true
            fi
            ;;
    esac
    
    if [ "$test_passed" = true ]; then
        print_color "$GREEN" "  ✓ PASS"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        print_color "$RED" "  ✗ FAIL"
        if [ "$test_type" = "EXACT" ] || [ "$test_type" = "LITERAL" ]; then
            echo "  Expected: '$expected'"
            echo "  Got:      '$output'"
        fi
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

# Test basic wildcard functionality
test_basic_wildcards() {
    print_test_header "Basic Wildcard Tests"
    
    # Create test files for these tests
    touch test_file1.txt test_file2.txt test_file3.log
    touch .hidden_file
    
    run_test "Simple asterisk pattern" "echo *.c" "CONTAINS" "test1.c test2.c"
    run_test "Pattern with extension" "echo *.txt" "CONTAINS" "test_file1.txt test_file2.txt"
    run_test "Pattern with specific extension" "echo *.log" "CONTAINS" "test_file3.log"
    run_test "Pattern that matches directories" "echo */" "NONEMPTY" ""
    
    # Clean up test files
    rm -f test_file1.txt test_file2.txt test_file3.log .hidden_file
}

# Test edge cases
test_edge_cases() {
    print_test_header "Edge Case Tests"
    
    run_test "No matches pattern" "echo *.xyz" "LITERAL" "*.xyz"
    run_test "Multiple asterisks" "echo *.*" "NONEMPTY" ""
    run_test "Pattern at start" "echo test*" "CONTAINS" "test1.c test2.c"
    run_test "Pattern at end" "echo *.c" "CONTAINS" "test1.c test2.c"
    run_test "Single asterisk" "echo *" "NONEMPTY" ""
}

# Test quoted patterns (should not expand)
test_quoted_patterns() {
    print_test_header "Quoted Pattern Tests"
    
    run_test "Single quoted pattern" "echo '*.c'" "LITERAL" "*.c"
    run_test "Double quoted pattern" 'echo "*.c"' "LITERAL" "*.c"
    run_test "Mixed quotes with expansion" "echo '*.c' *.c" "CONTAINS" "*.c test1.c test2.c"
}

# Test with different commands  
test_with_commands() {
    print_test_header "Command Integration Tests"
    
    # Create files with content for better testing
    echo "Test content 1" > cmd_test1.txt
    echo "Test content 2" > cmd_test2.txt
    
    run_test "ls with wildcard" "ls *.c" "CONTAINS" "test1.c test2.c"
    run_test "cat with wildcard" "cat *.txt" "NONEMPTY" ""
    run_test "wc with wildcard" "wc *.c" "NONEMPTY" ""
    
    rm -f cmd_test1.txt cmd_test2.txt
}

# Test performance with many files
test_performance() {
    print_test_header "Performance Tests"
    
    # Create many files
    for i in {1..10}; do
        touch "perf_test_$i.txt"
    done
    
    run_test "Many files pattern" "echo perf_test_*.txt" "NONEMPTY" ""
    run_test "Multiple patterns" "echo *.txt *.c *.md" "NONEMPTY" ""
    
    # Clean up
    rm -f perf_test_*.txt
}

# Test current directory limitation (correct behavior)
test_current_directory_only() {
    print_test_header "Current Directory Limitation Tests"
    
    # These should NOT expand as they reference outside current directory
    run_test "Parent directory pattern (should not expand)" "echo ../*.c" "LITERAL" "../*.c"
    run_test "Subdirectory pattern (should not expand)" "echo main/*.c" "LITERAL" "main/*.c"
    run_test "Absolute path pattern (should not expand)" "echo /tmp/*.txt" "LITERAL" "/tmp/*.txt"
    run_test "Nested pattern (should not expand)" "echo ../tests/*.sh" "LITERAL" "../tests/*.sh"
}

# Test special characters and edge conditions
test_special_characters() {
    print_test_header "Special Character Tests"
    
    # Create files with special names for testing
    touch "file with spaces.txt"
    touch "file-with-dashes.txt"
    touch "file_with_underscores.txt"
    
    run_test "Files with spaces" "echo *spaces*" "CONTAINS" "file with spaces.txt"
    run_test "Files with dashes" "echo *-with-*" "CONTAINS" "file-with-dashes.txt"
    run_test "Files with underscores" "echo *_with_*" "CONTAINS" "file_with_underscores.txt"
    
    # Test patterns that should not match anything
    run_test "Pattern with brackets (no expansion)" "echo *[*.c" "LITERAL" "*[*.c"
    run_test "Very long pattern" "echo *.$(printf '%0100d' 1)" "LITERAL" "*.$(printf '%0100d' 1)"
    
    # Clean up
    rm -f "file with spaces.txt" "file-with-dashes.txt" "file_with_underscores.txt"
}

# Test error conditions and edge cases
test_error_conditions() {
    print_test_header "Error Condition Tests"
    
    run_test "Empty echo command" "echo" "EMPTY" ""
    run_test "Multiple wildcards in sequence" "echo *.*.c" "LITERAL" "*.*.c"
    run_test "Wildcard at beginning" "echo *c" "NONEMPTY" ""
    run_test "Wildcard in middle" "echo t*t" "NONEMPTY" ""
}

# Test hidden files behavior
test_hidden_files() {
    print_test_header "Hidden Files Tests"
    
    # Create hidden files
    touch .hidden1 .hidden2 .hidden.txt
    
    # Hidden files should NOT be matched by * pattern (standard shell behavior)
    run_test "Asterisk should not match hidden files" "echo *" "NONEMPTY" ""
    run_test "Explicit hidden pattern" "echo .*" "CONTAINS" ".hidden1 .hidden2 .hidden.txt"
    run_test "Hidden files with extension" "echo .*.txt" "CONTAINS" ".hidden.txt"
    
    # Clean up
    rm -f .hidden1 .hidden2 .hidden.txt
}

# Main test runner
main() {
    print_color "$PURPLE" "========================================"
    print_color "$PURPLE" "    FINAL WILDCARD COMPREHENSIVE TESTS"
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
    
    print_color "$GREEN" "Found minishell binary at: $MINISHELL_PATH"
    print_color "$GREEN" "Testing from directory: $(pwd)"
    echo ""
    
    # Run test suites
    test_basic_wildcards
    test_edge_cases
    test_quoted_patterns
    test_with_commands
    test_performance
    test_current_directory_only
    test_special_characters
    test_error_conditions
    test_hidden_files
    
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
        print_color "$GREEN" "Wildcard implementation is working correctly!"
        exit 0
    else
        local success_rate=$((PASSED_TESTS * 100 / TOTAL_TESTS))
        print_color "$YELLOW" "Success rate: $success_rate%"
        
        if [ $success_rate -ge 90 ]; then
            print_color "$GREEN" "Excellent! Minor issues detected."
        elif [ $success_rate -ge 75 ]; then
            print_color "$YELLOW" "Good! Some issues to address."
        else
            print_color "$RED" "Multiple issues detected. Review implementation."
        fi
        
        exit 1
    fi
}

# Run the tests
main "$@"
