#!/bin/bash

# ************************************************************************** #
#                                                                            #
#                                                        :::      ::::::::   #
#   test_wildcard_comprehensive.sh                     :+:      :+:    :+:   #
#                                                    +:+ +:+         +:+     #
#   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        #
#                                                +#+#+#+#+#+   +#+           #
#   Created: 2025/07/14 00:00:00 by emgenc            #+#    #+#             #
#   Updated: 2025/07/14 00:00:00 by emgenc           ###   ########.fr       #
#                                                                            #
# ************************************************************************** #

# Comprehensive Wildcard Implementation Tester
# Tests all aspects of the wildcard functionality in current directory only

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

# Test directory setup
TEST_DIR="wildcard_test_env"
MINISHELL="../minishell"
TIMEOUT_DURATION=5

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

# Function to print test subheader
print_test_subheader() {
    local test_name=$1
    print_color "$BLUE" "--- $test_name ---"
}

# Function to increment test counters
increment_test() {
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
}

# Function to mark test as passed
test_passed() {
    PASSED_TESTS=$((PASSED_TESTS + 1))
    print_color "$GREEN" "✓ PASSED"
}

# Function to mark test as failed
test_failed() {
    local expected=$1
    local actual=$2
    FAILED_TESTS=$((FAILED_TESTS + 1))
    print_color "$RED" "✗ FAILED"
    if [ -n "$expected" ] && [ -n "$actual" ]; then
        print_color "$YELLOW" "Expected: $expected"
        print_color "$YELLOW" "Actual  : $actual"
    fi
}

# Function to run minishell command and capture output
run_minishell_test() {
    local command=$1
    local expected_pattern=$2
    local test_description=$3
    
    increment_test
    echo "Testing: $test_description"
    echo "Command: $command"
    
    # Run command with timeout and capture output
    local output
    output=$(echo "$command" | timeout $TIMEOUT_DURATION $MINISHELL 2>/dev/null | tail -n +2 | head -n -1)
    local exit_code=$?
    
    if [ $exit_code -eq 124 ]; then
        test_failed "No timeout" "Command timed out"
        return 1
    fi
    
    echo "Output: '$output'"
    
    if [ -n "$expected_pattern" ]; then
        if echo "$output" | grep -q "$expected_pattern"; then
            test_passed
            return 0
        else
            test_failed "$expected_pattern" "$output"
            return 1
        fi
    else
        # For tests where we just want to ensure no crash
        test_passed
        return 0
    fi
}

# Function to test wildcard expansion and compare with expected files
test_wildcard_expansion() {
    local pattern=$1
    local expected_files="$2"
    local test_description="$3"
    local should_include_hidden="$4"
    
    increment_test
    echo "Testing: $test_description"
    echo "Pattern: $pattern"
    
    # Get actual files matching pattern using shell expansion
    local actual_matches=""
    if [ "$should_include_hidden" = "true" ]; then
        # Include hidden files
        shopt -s dotglob
        actual_matches=$(ls -1 $pattern 2>/dev/null | sort | tr '\n' ' ' | sed 's/ $//')
        shopt -u dotglob
    else
        # Exclude hidden files (default behavior)
        actual_matches=$(ls -1 $pattern 2>/dev/null | grep -v '^\.' | sort | tr '\n' ' ' | sed 's/ $//')
    fi
    
    # Run minishell command
    local command="echo $pattern"
    local output
    output=$(echo "$command" | timeout $TIMEOUT_DURATION $MINISHELL 2>/dev/null | tail -n +2 | head -n -1)
    
    # Extract just the filenames from the quoted output
    local minishell_matches
    minishell_matches=$(echo "$output" | sed "s/'/ /g" | tr ' ' '\n' | grep -v '^$' | sort | tr '\n' ' ' | sed 's/ $//')
    
    echo "Expected files: '$actual_matches'"
    echo "Minishell output: '$output'"
    echo "Minishell matches: '$minishell_matches'"
    
    if [ "$actual_matches" = "$minishell_matches" ]; then
        test_passed
        return 0
    else
        test_failed "$actual_matches" "$minishell_matches"
        return 1
    fi
}

# Function to create test environment
setup_test_environment() {
    print_test_header "SETTING UP TEST ENVIRONMENT"
    
    # Remove existing test directory
    rm -rf "$TEST_DIR"
    mkdir -p "$TEST_DIR"
    cd "$TEST_DIR"
    
    # Create various test files and directories
    echo "Creating test files..."
    
    # Regular files
    touch "file1.txt"
    touch "file2.txt"
    touch "file3.c"
    touch "test.c"
    touch "test.h"
    touch "main.c"
    touch "main.h"
    touch "program"
    touch "script.sh"
    touch "data.json"
    touch "config.yml"
    touch "readme.md"
    touch "makefile"
    touch "Makefile"
    
    # Files with special characters and numbers
    touch "file_with_underscore.txt"
    touch "file-with-dash.txt"
    touch "file123.txt"
    touch "123file.txt"
    touch "file.backup.txt"
    touch "file.old"
    touch "temp1"
    touch "temp2"
    touch "temp10"
    touch "temp_file"
    
    # Hidden files (should be filtered by default)
    touch ".hidden"
    touch ".hiddenfile.txt"
    touch ".gitignore"
    touch ".vscode"
    touch ".config"
    
    # Files with spaces and special names
    touch "file with spaces.txt"
    touch "very_long_filename_with_many_characters.extension"
    touch "a"
    touch "ab"
    touch "abc"
    touch "abcd"
    
    # Files with multiple extensions
    touch "archive.tar.gz"
    touch "backup.sql.bak"
    touch "image.jpg"
    touch "image.png"
    
    # Empty filename cases
    touch "."
    touch ".."
    
    # Create subdirectories (should not be matched differently)
    mkdir -p subdir1
    mkdir -p subdir2
    mkdir -p .hidden_dir
    
    # Files in subdirectories (should not be matched by current dir wildcards)
    touch subdir1/nested_file.txt
    touch subdir2/another_file.c
    
    echo "Test environment created with $(ls -la | wc -l) entries"
    ls -la
    
    print_color "$GREEN" "Test environment setup complete!"
}

# Function to cleanup test environment
cleanup_test_environment() {
    cd ..
    rm -rf "$TEST_DIR"
    print_color "$GREEN" "Test environment cleaned up!"
}

# Test basic wildcard patterns
test_basic_wildcards() {
    print_test_header "BASIC WILDCARD PATTERNS"
    
    print_test_subheader "Single asterisk - should match all non-hidden files"
    test_wildcard_expansion "*" "" "Match all files with *"
    
    print_test_subheader "Asterisk with extension"
    test_wildcard_expansion "*.txt" "" "Match all .txt files"
    test_wildcard_expansion "*.c" "" "Match all .c files"
    test_wildcard_expansion "*.h" "" "Match all .h files"
    test_wildcard_expansion "*.sh" "" "Match all .sh files"
    test_wildcard_expansion "*.nonexistent" "" "Match files with non-existent extension"
    
    print_test_subheader "Asterisk with prefix"
    test_wildcard_expansion "file*" "" "Match files starting with 'file'"
    test_wildcard_expansion "test*" "" "Match files starting with 'test'"
    test_wildcard_expansion "main*" "" "Match files starting with 'main'"
    test_wildcard_expansion "temp*" "" "Match files starting with 'temp'"
    test_wildcard_expansion "nonexistent*" "" "Match files starting with 'nonexistent'"
    
    print_test_subheader "Prefix and suffix combination"
    test_wildcard_expansion "file*.txt" "" "Match files starting with 'file' and ending with '.txt'"
    test_wildcard_expansion "test*.c" "" "Match files starting with 'test' and ending with '.c'"
    test_wildcard_expansion "main*.h" "" "Match files starting with 'main' and ending with '.h'"
}

# Test multiple asterisks
test_multiple_asterisks() {
    print_test_header "MULTIPLE ASTERISK PATTERNS"
    
    print_test_subheader "Double asterisks"
    test_wildcard_expansion "**" "" "Double asterisk pattern"
    test_wildcard_expansion "***" "" "Triple asterisk pattern"
    test_wildcard_expansion "****" "" "Quadruple asterisk pattern"
    
    print_test_subheader "Asterisks with text segments"
    test_wildcard_expansion "*.*" "" "Pattern with asterisks around dot"
    test_wildcard_expansion "*file*" "" "Pattern with asterisks around 'file'"
    test_wildcard_expansion "*test*" "" "Pattern with asterisks around 'test'"
    test_wildcard_expansion "f*e*" "" "Pattern starting with 'f' and having 'e' in middle"
    test_wildcard_expansion "*1*2*" "" "Pattern with digits separated by asterisks"
    
    print_test_subheader "Complex multi-asterisk patterns"
    test_wildcard_expansion "f*l*e*.txt" "" "Complex pattern: f*l*e*.txt"
    test_wildcard_expansion "*a*b*c*" "" "Pattern matching files with a, b, c in sequence"
    test_wildcard_expansion "*.*.*" "" "Pattern for files with double extensions"
}

# Test edge cases
test_edge_cases() {
    print_test_header "EDGE CASES"
    
    print_test_subheader "Empty and minimal patterns"
    run_minishell_test "echo ''" "" "Empty pattern"
    run_minishell_test "echo " "" "No pattern"
    
    print_test_subheader "Hidden file handling"
    # Hidden files should NOT be matched by default wildcard patterns
    test_wildcard_expansion ".*" "" "Hidden files pattern (should match hidden files)" "true"
    test_wildcard_expansion ".hidden*" "" "Pattern starting with .hidden" "true"
    test_wildcard_expansion "*.txt" "" "Normal pattern should not match hidden .txt files"
    
    print_test_subheader "Case sensitivity"
    test_wildcard_expansion "makefile*" "" "Lowercase makefile"
    test_wildcard_expansion "Makefile*" "" "Uppercase Makefile"
    test_wildcard_expansion "*MAKEFILE*" "" "All uppercase pattern"
    
    print_test_subheader "Single character patterns"
    test_wildcard_expansion "a*" "" "Files starting with 'a'"
    test_wildcard_expansion "*a" "" "Files ending with 'a'"
    test_wildcard_expansion "a" "" "Exact match 'a'"
    
    print_test_subheader "Number patterns"
    test_wildcard_expansion "*1*" "" "Files containing '1'"
    test_wildcard_expansion "1*" "" "Files starting with '1'"
    test_wildcard_expansion "*1" "" "Files ending with '1'"
    test_wildcard_expansion "*123*" "" "Files containing '123'"
}

# Test with special characters
test_special_characters() {
    print_test_header "SPECIAL CHARACTER HANDLING"
    
    print_test_subheader "Underscore and dash patterns"
    test_wildcard_expansion "*_*" "" "Files containing underscore"
    test_wildcard_expansion "*-*" "" "Files containing dash"
    test_wildcard_expansion "file_*" "" "Files starting with 'file_'"
    test_wildcard_expansion "file-*" "" "Files starting with 'file-'"
    
    print_test_subheader "Space handling"
    test_wildcard_expansion "*with*" "" "Files containing 'with'"
    test_wildcard_expansion "file*with*" "" "Files starting with 'file' and containing 'with'"
    
    print_test_subheader "Dot patterns"
    test_wildcard_expansion "*.*" "" "Files with extensions (containing dots)"
    test_wildcard_expansion "*.backup.*" "" "Files with .backup. in name"
    test_wildcard_expansion "*old*" "" "Files containing 'old'"
}

# Test quote handling
test_quote_handling() {
    print_test_header "QUOTE HANDLING"
    
    print_test_subheader "Single quotes"
    run_minishell_test "echo '*'" "*" "Single quoted asterisk should not expand"
    run_minishell_test "echo '*.txt'" "*.txt" "Single quoted pattern should not expand"
    run_minishell_test "echo 'file*'" "file*" "Single quoted prefix pattern should not expand"
    
    print_test_subheader "Double quotes"
    run_minishell_test 'echo "*"' "*" "Double quoted asterisk should not expand"
    run_minishell_test 'echo "*.txt"' "*.txt" "Double quoted pattern should not expand"
    run_minishell_test 'echo "file*"' "file*" "Double quoted prefix pattern should not expand"
    
    print_test_subheader "Mixed quotes"
    run_minishell_test "echo '*'.txt" "" "Mixed quoting: '*'.txt"
    run_minishell_test "echo file'*'" "" "Mixed quoting: file'*'"
}

# Test no matches scenarios
test_no_matches() {
    print_test_header "NO MATCHES SCENARIOS"
    
    print_test_subheader "Non-existent patterns"
    test_wildcard_expansion "*.xyz" "" "Non-existent extension"
    test_wildcard_expansion "notfound*" "" "Non-existent prefix"
    test_wildcard_expansion "*notfound" "" "Non-existent suffix"
    test_wildcard_expansion "xyz*abc" "" "Non-existent complex pattern"
    
    print_test_subheader "Impossible patterns"
    test_wildcard_expansion "a*b*c*d*e*f*g*h*i*j*k*l*m*n*o*p*q*r*s*t*u*v*w*x*y*z*" "" "Very long impossible pattern"
    test_wildcard_expansion "file1.txt.exe" "" "Exact non-existent filename"
}

# Test directory behavior
test_directory_behavior() {
    print_test_header "DIRECTORY BEHAVIOR"
    
    print_test_subheader "Directory matching"
    test_wildcard_expansion "sub*" "" "Directories starting with 'sub'"
    test_wildcard_expansion "*dir*" "" "Entries containing 'dir'"
    
    print_test_subheader "Current and parent directory"
    # Note: . and .. should be handled specially
    test_wildcard_expansion ".*" "" "Pattern matching . and .." "true"
    
    print_test_subheader "Hidden directories"
    test_wildcard_expansion ".hidden*" "" "Hidden directories pattern" "true"
}

# Test performance with many files
test_performance() {
    print_test_header "PERFORMANCE TESTS"
    
    print_test_subheader "Creating many files for performance test"
    for i in {1..100}; do
        touch "perf_file_$i.txt"
        touch "test_perf_$i.c"
    done
    
    print_test_subheader "Performance with many matches"
    local start_time=$(date +%s.%N)
    test_wildcard_expansion "perf_*" "" "Pattern matching many files (100 files)"
    local end_time=$(date +%s.%N)
    local duration=$(echo "$end_time - $start_time" | bc)
    echo "Duration: ${duration}s"
    
    print_test_subheader "Performance with complex pattern"
    start_time=$(date +%s.%N)
    test_wildcard_expansion "*perf*" "" "Complex pattern with many matches"
    end_time=$(date +%s.%N)
    duration=$(echo "$end_time - $start_time" | bc)
    echo "Duration: ${duration}s"
    
    # Cleanup performance test files
    rm -f perf_file_*.txt test_perf_*.c
}

# Test error conditions
test_error_conditions() {
    print_test_header "ERROR CONDITIONS AND RESILIENCE"
    
    print_test_subheader "Very long patterns"
    local long_pattern=$(printf '*%.0s' {1..1000})
    run_minishell_test "echo $long_pattern" "" "Very long wildcard pattern"
    
    print_test_subheader "Many consecutive asterisks"
    local many_asterisks=$(printf '*%.0s' {1..50})
    run_minishell_test "echo $many_asterisks" "" "Many consecutive asterisks"
    
    print_test_subheader "Patterns with null characters"
    # This is tricky to test, but we can try some edge cases
    run_minishell_test "echo \*" "" "Escaped asterisk"
    
    print_test_subheader "Memory stress test"
    run_minishell_test "echo *a*b*c*d*e*f*g*h*i*j*k*l*m*n*o*p*q*r*s*t*u*v*w*x*y*z*" "" "Complex pattern for stress test"
}

# Test integration with shell features
test_integration() {
    print_test_header "INTEGRATION WITH SHELL FEATURES"
    
    print_test_subheader "Wildcards in commands"
    run_minishell_test "ls *.txt" "" "ls command with wildcard"
    run_minishell_test "cat *.c" "" "cat command with wildcard (may fail if no .c files)"
    
    print_test_subheader "Wildcards with pipes"
    run_minishell_test "echo *.txt | cat" "" "Wildcard with pipe"
    
    print_test_subheader "Wildcards with redirections"
    run_minishell_test "echo *.txt > wildcard_output.tmp" "" "Wildcard with output redirection"
    run_minishell_test "cat wildcard_output.tmp" "" "Reading wildcard output"
    rm -f wildcard_output.tmp
    
    print_test_subheader "Multiple wildcards in one command"
    run_minishell_test "echo *.txt *.c" "" "Multiple wildcard patterns"
    run_minishell_test "echo *.txt file* test*" "" "Mixed wildcard and literal patterns"
}

# Test current directory limitation
test_current_directory_limitation() {
    print_test_header "CURRENT DIRECTORY LIMITATION TESTS"
    
    print_test_subheader "Subdirectory patterns should not work"
    run_minishell_test "echo subdir1/*" "subdir1/*" "Pattern in subdirectory should not expand"
    run_minishell_test "echo */file*" "*/file*" "Pattern with subdirectory wildcard should not expand"
    run_minishell_test "echo ./*" "" "Pattern with explicit current directory"
    
    print_test_subheader "Path patterns"
    run_minishell_test "echo ../*" "../*" "Parent directory pattern should not expand"
    run_minishell_test "echo ~/*" "~/*" "Home directory pattern should not expand"
    run_minishell_test "echo /tmp/*" "/tmp/*" "Absolute path pattern should not expand"
}

# Comprehensive pattern validation
test_comprehensive_patterns() {
    print_test_header "COMPREHENSIVE PATTERN VALIDATION"
    
    # Test every file individually to ensure proper matching
    print_test_subheader "Individual file pattern tests"
    
    # Get all non-hidden files
    local files=($(ls -1 | grep -v '^\.'))
    
    for file in "${files[@]}"; do
        if [ -f "$file" ]; then
            # Test exact match
            increment_test
            echo "Testing exact match for: $file"
            local output=$(echo "echo $file" | timeout $TIMEOUT_DURATION $MINISHELL 2>/dev/null | tail -n +2 | head -n -1)
            if echo "$output" | grep -q "$file"; then
                test_passed
            else
                test_failed "$file" "$output"
            fi
            
            # Test wildcard pattern that should match this file
            if [[ "$file" == *.* ]]; then
                local ext="${file##*.}"
                increment_test
                echo "Testing extension pattern *.$ext for: $file"
                output=$(echo "echo *.$ext" | timeout $TIMEOUT_DURATION $MINISHELL 2>/dev/null | tail -n +2 | head -n -1)
                if echo "$output" | grep -q "$file"; then
                    test_passed
                else
                    test_failed "Should contain $file" "$output"
                fi
            fi
        fi
    done
}

# Main test execution
main() {
    print_color "$PURPLE" "==============================================="
    print_color "$PURPLE" "    MINISHELL WILDCARD COMPREHENSIVE TESTER"
    print_color "$PURPLE" "==============================================="
    echo ""
    
    # Check if minishell binary exists
    if [ ! -f "$MINISHELL" ]; then
        print_color "$RED" "Error: Minishell binary not found at $MINISHELL"
        print_color "$YELLOW" "Please make sure minishell is compiled and the path is correct."
        exit 1
    fi
    
    print_color "$GREEN" "Using minishell binary: $MINISHELL"
    print_color "$GREEN" "Timeout duration: ${TIMEOUT_DURATION}s"
    echo ""
    
    # Setup test environment
    setup_test_environment
    
    # Run all test suites
    test_basic_wildcards
    test_multiple_asterisks
    test_edge_cases
    test_special_characters
    test_quote_handling
    test_no_matches
    test_directory_behavior
    test_performance
    test_error_conditions
    test_integration
    test_current_directory_limitation
    test_comprehensive_patterns
    
    # Cleanup
    cleanup_test_environment
    
    # Print final results
    print_test_header "FINAL RESULTS"
    echo ""
    print_color "$CYAN" "Total Tests Run: $TOTAL_TESTS"
    print_color "$GREEN" "Tests Passed: $PASSED_TESTS"
    print_color "$RED" "Tests Failed: $FAILED_TESTS"
    
    if [ $FAILED_TESTS -eq 0 ]; then
        print_color "$GREEN" "🎉 ALL TESTS PASSED! 🎉"
        print_color "$GREEN" "Wildcard implementation is working correctly!"
        exit 0
    else
        local success_rate=$((PASSED_TESTS * 100 / TOTAL_TESTS))
        print_color "$YELLOW" "Success Rate: ${success_rate}%"
        print_color "$RED" "Some tests failed. Please review the implementation."
        exit 1
    fi
}

# Check if bc is available for performance timing
if ! command -v bc &> /dev/null; then
    print_color "$YELLOW" "Warning: 'bc' not found. Performance timing will be skipped."
fi

# Run main function
main "$@"
