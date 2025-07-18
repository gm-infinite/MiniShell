#!/bin/bash

# ************************************************************************** #
#                                                                            #
#                                                        :::      ::::::::   #
#   test_wildcard_stress.sh                            :+:      :+:    :+:   #
#                                                    +:+ +:+         +:+     #
#   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        #
#                                                +#+#+#+#+#+   +#+           #
#   Created: 2025/07/14 00:00:00 by emgenc            #+#    #+#             #
#   Updated: 2025/07/14 00:00:00 by emgenc           ###   ########.fr       #
#                                                                            #
# ************************************************************************** #

# Wildcard Stress Testing Script
# Tests edge cases, memory conditions, and boundary scenarios

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

# Configuration
MINISHELL="../minishell"
TEST_DIR="wildcard_stress_test"
TIMEOUT_DURATION=10
MAX_FILES=1000

# Counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

print_color() {
    echo -e "${1}${2}${NC}"
}

print_header() {
    echo ""
    print_color "$CYAN" "========== $1 =========="
}

increment_test() {
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
}

test_passed() {
    PASSED_TESTS=$((PASSED_TESTS + 1))
    print_color "$GREEN" "✓ PASSED"
}

test_failed() {
    FAILED_TESTS=$((FAILED_TESTS + 1))
    print_color "$RED" "✗ FAILED: $1"
}

run_stress_test() {
    local command="$1"
    local description="$2"
    local expected_behavior="$3"
    
    increment_test
    echo "Testing: $description"
    echo "Command: $command"
    
    local start_time=$(date +%s.%N)
    local output
    local exit_code
    
    # Run with timeout and capture both stdout and stderr
    output=$(echo "$command" | timeout $TIMEOUT_DURATION $MINISHELL 2>&1)
    exit_code=$?
    local end_time=$(date +%s.%N)
    
    # Calculate duration if bc is available
    local duration="N/A"
    if command -v bc &> /dev/null; then
        duration=$(echo "$end_time - $start_time" | bc)
    fi
    
    echo "Exit code: $exit_code"
    echo "Duration: ${duration}s"
    echo "Output: '$output'"
    
    case "$expected_behavior" in
        "no_crash")
            if [ $exit_code -ne 124 ] && [ $exit_code -ne 139 ] && [ $exit_code -ne 11 ]; then
                test_passed
            else
                test_failed "Program crashed or timed out"
            fi
            ;;
        "timeout_ok")
            if [ $exit_code -eq 124 ]; then
                test_passed
            else
                test_failed "Expected timeout but command completed"
            fi
            ;;
        "no_timeout")
            if [ $exit_code -ne 124 ]; then
                test_passed
            else
                test_failed "Command timed out unexpectedly"
            fi
            ;;
        *)
            test_passed
            ;;
    esac
}

setup_stress_environment() {
    print_header "SETTING UP STRESS TEST ENVIRONMENT"
    
    rm -rf "$TEST_DIR"
    mkdir -p "$TEST_DIR"
    cd "$TEST_DIR"
    
    print_color "$YELLOW" "Creating various file types..."
    
    # Create files with extreme names
    touch "a"
    touch "ab"
    touch "abc"
    touch "abcdefghijklmnopqrstuvwxyz"
    touch "verylongfilenamethatgoesonsandonandtestsmemorylimits.txt"
    
    # Files with special characters
    touch "file!@#$%^&()_+-={}[]|\\:;\"'<>,.?~\`.txt"
    touch "file\$with\$dollar\$signs.txt"
    
    # Files with unicode characters (if supported)
    touch "файл.txt" 2>/dev/null || true
    touch "😀.txt" 2>/dev/null || true
    
    # Files that might cause parsing issues
    touch "file*.txt"
    touch "file[].txt"
    touch "file{}.txt"
    touch "file().txt"
    
    # Hidden files with complex patterns
    touch ".hidden_file_with_very_long_name_that_tests_hidden_file_handling.txt"
    touch ".a"
    touch "..hidden"
    
    # Files with numbers in various positions
    for i in {0..9}; do
        touch "${i}file.txt"
        touch "file${i}.txt"
        touch "file${i}${i}.txt"
    done
    
    # Files with multiple dots
    touch "file.name.with.many.dots.final.extension"
    touch "...dotfile"
    touch "file."
    touch ".file."
    
    echo "Created stress test environment with $(ls -1 | wc -l) files"
}

test_memory_stress() {
    print_header "MEMORY STRESS TESTS"
    
    print_color "$BLUE" "--- Creating many files for memory stress ---"
    for i in $(seq 1 200); do
        touch "stress_file_${i}.txt"
    done
    
    run_stress_test "echo *" "Wildcard with 200+ files" "no_crash"
    run_stress_test "echo *.txt" "Extension wildcard with many matches" "no_crash"
    run_stress_test "echo stress_*" "Prefix wildcard with many matches" "no_crash"
    run_stress_test "echo *stress*" "Infix wildcard with many matches" "no_crash"
    
    # Cleanup stress files
    rm -f stress_file_*.txt
}

test_pattern_complexity() {
    print_header "PATTERN COMPLEXITY STRESS TESTS"
    
    print_color "$BLUE" "--- Testing extremely complex patterns ---"
    
    # Very long patterns
    local long_pattern=""
    for i in {1..100}; do
        long_pattern="${long_pattern}*"
    done
    run_stress_test "echo $long_pattern" "100 consecutive asterisks" "no_crash"
    
    # Alternating patterns
    local alt_pattern=""
    for i in {1..50}; do
        alt_pattern="${alt_pattern}a*"
    done
    run_stress_test "echo $alt_pattern" "Alternating a and * pattern" "no_crash"
    
    # Complex multi-segment patterns
    run_stress_test "echo *a*b*c*d*e*f*g*h*i*j*k*l*m*n*o*p*q*r*s*t*u*v*w*x*y*z*" "Alphabet pattern" "no_crash"
    run_stress_test "echo *1*2*3*4*5*6*7*8*9*0*" "Number pattern" "no_crash"
    
    # Patterns with special characters
    run_stress_test "echo *.*.*.*.*.*" "Multiple dot patterns" "no_crash"
    run_stress_test "echo *_*_*_*_*" "Multiple underscore patterns" "no_crash"
    run_stress_test "echo *-*-*-*-*" "Multiple dash patterns" "no_crash"
}

test_boundary_conditions() {
    print_header "BOUNDARY CONDITION TESTS"
    
    print_color "$BLUE" "--- Testing boundary conditions ---"
    
    # Empty directory test
    mkdir empty_test_dir
    cd empty_test_dir
    run_stress_test "echo *" "Wildcard in empty directory" "no_crash"
    run_stress_test "echo *.txt" "Extension wildcard in empty directory" "no_crash"
    cd ..
    rmdir empty_test_dir
    
    # Single character tests
    run_stress_test "echo ?" "Question mark (not wildcard)" "no_crash"
    run_stress_test "echo [" "Single bracket" "no_crash"
    run_stress_test "echo ]" "Single bracket close" "no_crash"
    run_stress_test "echo {" "Single brace" "no_crash"
    run_stress_test "echo }" "Single brace close" "no_crash"
    
    # Control characters
    run_stress_test "echo '\t*'" "Tab with wildcard" "no_crash"
    run_stress_test "echo '\n*'" "Newline with wildcard" "no_crash"
    
    # Null and whitespace
    run_stress_test "echo ' * '" "Wildcard with spaces" "no_crash"
    run_stress_test "echo '  *  '" "Wildcard with multiple spaces" "no_crash"
}

test_concurrent_operations() {
    print_header "CONCURRENT OPERATION SIMULATION"
    
    print_color "$BLUE" "--- Testing rapid succession commands ---"
    
    # Rapid fire tests
    for i in {1..10}; do
        run_stress_test "echo *" "Rapid test $i" "no_crash" &
    done
    wait
    
    # Create and delete files during wildcard operations
    print_color "$BLUE" "--- Testing with file system changes ---"
    
    # Create files in background while running wildcards
    (
        for i in {1..50}; do
            touch "dynamic_$i.tmp"
            sleep 0.01
        done
    ) &
    
    sleep 0.1
    run_stress_test "echo *.tmp" "Wildcard during file creation" "no_crash"
    
    wait
    rm -f dynamic_*.tmp
}

test_error_recovery() {
    print_header "ERROR RECOVERY TESTS"
    
    print_color "$BLUE" "--- Testing error conditions ---"
    
    # Invalid file operations during wildcard
    chmod 000 . 2>/dev/null
    run_stress_test "echo *" "Wildcard with no read permission" "no_crash"
    chmod 755 .
    
    # Simulate disk full (if possible)
    # This is environment-dependent and may not work everywhere
    
    # Test with corrupted file names (non-printable characters)
    printf "test\x00file" > weird_file 2>/dev/null || true
    run_stress_test "echo *" "Wildcard with weird filename" "no_crash"
    rm -f weird_file
    
    # Test with very long filename
    local long_name=$(printf 'a%.0s' {1..255})
    touch "$long_name" 2>/dev/null || true
    run_stress_test "echo $long_name*" "Very long filename pattern" "no_crash"
    rm -f "$long_name"
}

test_memory_limits() {
    print_header "MEMORY LIMIT TESTS"
    
    print_color "$BLUE" "--- Testing memory allocation limits ---"
    
    # Create files to test memory allocation
    print_color "$YELLOW" "Creating files for memory test..."
    for i in $(seq 1 500); do
        touch "mem_test_file_with_very_long_name_${i}_$(printf 'x%.0s' {1..50}).txt"
    done
    
    run_stress_test "echo mem_test_*" "500 files with long names" "no_crash"
    run_stress_test "echo *very_long_name*" "Pattern matching long names" "no_crash"
    
    # Test with pattern that would generate huge output
    run_stress_test "echo *" "All files (500+ matches)" "no_crash"
    
    # Cleanup memory test files
    rm -f mem_test_*.txt
}

test_filesystem_edge_cases() {
    print_header "FILESYSTEM EDGE CASES"
    
    print_color "$BLUE" "--- Testing filesystem edge cases ---"
    
    # Symbolic links
    touch real_file.txt
    ln -s real_file.txt symlink.txt 2>/dev/null || true
    ln -s nonexistent.txt broken_symlink.txt 2>/dev/null || true
    
    run_stress_test "echo *.txt" "Wildcards with symlinks" "no_crash"
    run_stress_test "echo symlink*" "Pattern matching symlinks" "no_crash"
    
    # Cleanup symlinks
    rm -f real_file.txt symlink.txt broken_symlink.txt
    
    # Named pipes (FIFOs)
    mkfifo test_pipe 2>/dev/null || true
    run_stress_test "echo test_*" "Pattern matching named pipe" "no_crash"
    rm -f test_pipe
    
    # Device files (if running with sufficient permissions)
    # This is usually not possible in user space, so we skip it
    
    # Files with setuid/setgid bits
    touch special_file
    chmod 4755 special_file 2>/dev/null || true
    run_stress_test "echo special_*" "Pattern matching special permission files" "no_crash"
    rm -f special_file
}

cleanup_stress_environment() {
    cd ..
    rm -rf "$TEST_DIR"
    print_color "$GREEN" "Stress test environment cleaned up!"
}

main() {
    print_color "$PURPLE" "================================================"
    print_color "$PURPLE" "    MINISHELL WILDCARD STRESS TESTER"
    print_color "$PURPLE" "================================================"
    
    if [ ! -f "$MINISHELL" ]; then
        print_color "$RED" "Error: Minishell binary not found at $MINISHELL"
        exit 1
    fi
    
    print_color "$GREEN" "Using minishell: $MINISHELL"
    print_color "$GREEN" "Timeout: ${TIMEOUT_DURATION}s"
    print_color "$YELLOW" "Warning: Some tests may take time or appear to hang - this is expected"
    echo ""
    
    setup_stress_environment
    
    test_memory_stress
    test_pattern_complexity
    test_boundary_conditions
    test_concurrent_operations
    test_error_recovery
    test_memory_limits
    test_filesystem_edge_cases
    
    cleanup_stress_environment
    
    # Final results
    print_header "STRESS TEST RESULTS"
    print_color "$CYAN" "Total stress tests: $TOTAL_TESTS"
    print_color "$GREEN" "Passed: $PASSED_TESTS"
    print_color "$RED" "Failed: $FAILED_TESTS"
    
    if [ $FAILED_TESTS -eq 0 ]; then
        print_color "$GREEN" "🎉 ALL STRESS TESTS PASSED! 🎉"
        print_color "$GREEN" "Wildcard implementation is robust!"
        exit 0
    else
        local success_rate=$((PASSED_TESTS * 100 / TOTAL_TESTS))
        print_color "$YELLOW" "Success rate: ${success_rate}%"
        print_color "$RED" "Some stress tests failed - review for robustness"
        exit 1
    fi
}

main "$@"
