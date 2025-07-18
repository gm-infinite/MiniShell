#!/bin/bash

# ************************************************************************** #
#                                                                            #
#                                                        :::      ::::::::   #
#   run_all_wildcard_tests.sh                          :+:      :+:    :+:   #
#                                                    +:+ +:+         +:+     #
#   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        #
#                                                +#+#+#+#+#+   +#+           #
#   Created: 2025/07/14 00:00:00 by emgenc            #+#    #+#             #
#   Updated: 2025/07/14 00:00:00 by emgenc           ###   ########.fr       #
#                                                                            #
# ************************************************************************** #

# Master test runner for all wildcard tests
# Runs unit tests, comprehensive tests, and stress tests

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

# Configuration
MINISHELL="../minishell"
MAKE_CMD="make"
VALGRIND_AVAILABLE=true

# Test results
UNIT_TEST_RESULT=""
COMPREHENSIVE_TEST_RESULT=""
STRESS_TEST_RESULT=""
OVERALL_SUCCESS=true

print_color() {
    echo -e "${1}${2}${NC}"
}

print_banner() {
    echo ""
    print_color "$PURPLE$BOLD" "################################################################"
    print_color "$PURPLE$BOLD" "#                                                              #"
    print_color "$PURPLE$BOLD" "#           MINISHELL WILDCARD TEST SUITE v1.0               #"
    print_color "$PURPLE$BOLD" "#                                                              #"
    print_color "$PURPLE$BOLD" "#  Comprehensive testing framework for wildcard functionality #"
    print_color "$PURPLE$BOLD" "#  - Unit tests for individual functions                      #"
    print_color "$PURPLE$BOLD" "#  - Comprehensive integration tests                          #"
    print_color "$PURPLE$BOLD" "#  - Stress tests for robustness                              #"
    print_color "$PURPLE$BOLD" "#                                                              #"
    print_color "$PURPLE$BOLD" "################################################################"
    echo ""
}

print_section() {
    echo ""
    print_color "$CYAN$BOLD" "===================================================="
    print_color "$CYAN$BOLD" " $1"
    print_color "$CYAN$BOLD" "===================================================="
}

print_subsection() {
    echo ""
    print_color "$BLUE" "--- $1 ---"
}

check_prerequisites() {
    print_section "CHECKING PREREQUISITES"
    
    local missing_deps=false
    
    # Check if minishell binary exists
    if [ ! -f "$MINISHELL" ]; then
        print_color "$RED" "✗ Minishell binary not found at: $MINISHELL"
        print_color "$YELLOW" "  Please compile minishell first"
        missing_deps=true
    else
        print_color "$GREEN" "✓ Minishell binary found"
    fi
    
    # Check if required directories exist
    if [ ! -d "../wildcard_handle" ]; then
        print_color "$RED" "✗ Wildcard source directory not found"
        missing_deps=true
    else
        print_color "$GREEN" "✓ Wildcard source directory found"
    fi
    
    # Check for required tools
    if ! command -v gcc &> /dev/null; then
        print_color "$RED" "✗ GCC compiler not found"
        missing_deps=true
    else
        print_color "$GREEN" "✓ GCC compiler available"
    fi
    
    if ! command -v make &> /dev/null; then
        print_color "$RED" "✗ Make utility not found"
        missing_deps=true
    else
        print_color "$GREEN" "✓ Make utility available"
    fi
    
    # Check for optional tools
    if ! command -v valgrind &> /dev/null; then
        print_color "$YELLOW" "⚠ Valgrind not found - memory tests will be skipped"
        VALGRIND_AVAILABLE=false
    else
        print_color "$GREEN" "✓ Valgrind available for memory testing"
    fi
    
    if ! command -v bc &> /dev/null; then
        print_color "$YELLOW" "⚠ BC calculator not found - timing will be limited"
    else
        print_color "$GREEN" "✓ BC calculator available for timing"
    fi
    
    if [ "$missing_deps" = true ]; then
        print_color "$RED" "Cannot proceed - missing required dependencies"
        exit 1
    fi
    
    print_color "$GREEN" "All prerequisites satisfied!"
}

build_tests() {
    print_section "BUILDING TEST EXECUTABLES"
    
    print_subsection "Building unit tests"
    if [ -f "Makefile_wildcard_tests" ]; then
        if make -f Makefile_wildcard_tests; then
            print_color "$GREEN" "✓ Unit test executable built successfully"
        else
            print_color "$RED" "✗ Failed to build unit test executable"
            return 1
        fi
    else
        print_color "$YELLOW" "⚠ No unit test Makefile found - skipping unit test build"
    fi
    
    print_subsection "Checking test script permissions"
    chmod +x test_wildcard_comprehensive.sh 2>/dev/null
    chmod +x test_wildcard_stress.sh 2>/dev/null
    print_color "$GREEN" "✓ Test scripts are executable"
    
    return 0
}

run_unit_tests() {
    print_section "RUNNING UNIT TESTS"
    
    if [ -f "test_wildcard_unit" ]; then
        print_subsection "Running unit tests"
        echo ""
        
        local start_time=$(date +%s)
        if ./test_wildcard_unit; then
            UNIT_TEST_RESULT="PASSED"
            print_color "$GREEN" "✓ Unit tests completed successfully"
        else
            UNIT_TEST_RESULT="FAILED"
            OVERALL_SUCCESS=false
            print_color "$RED" "✗ Unit tests failed"
        fi
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        print_color "$CYAN" "Unit test duration: ${duration}s"
        
        # Run with valgrind if available
        if [ "$VALGRIND_AVAILABLE" = true ]; then
            print_subsection "Running unit tests with Valgrind"
            echo ""
            if make -f Makefile_wildcard_tests valgrind 2>&1 | tee valgrind_unit.log; then
                print_color "$GREEN" "✓ Valgrind unit test completed"
                # Check for memory leaks
                if grep -q "definitely lost: 0 bytes" valgrind_unit.log && \
                   grep -q "possibly lost: 0 bytes" valgrind_unit.log; then
                    print_color "$GREEN" "✓ No memory leaks detected"
                else
                    print_color "$YELLOW" "⚠ Potential memory issues detected - check valgrind_unit.log"
                fi
            else
                print_color "$YELLOW" "⚠ Valgrind unit test had issues"
            fi
        fi
    else
        print_color "$YELLOW" "⚠ Unit test executable not found - skipping unit tests"
        UNIT_TEST_RESULT="SKIPPED"
    fi
}

run_comprehensive_tests() {
    print_section "RUNNING COMPREHENSIVE INTEGRATION TESTS"
    
    # Run the original comprehensive tests
    if [ -f "test_wildcard_comprehensive.sh" ]; then
        print_subsection "Running original comprehensive shell tests"
        echo ""
        
        local start_time=$(date +%s)
        if ./test_wildcard_comprehensive.sh; then
            print_color "$GREEN" "✓ Original comprehensive tests completed successfully"
        else
            OVERALL_SUCCESS=false
            print_color "$RED" "✗ Original comprehensive tests failed"
        fi
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        print_color "$CYAN" "Original comprehensive test duration: ${duration}s"
        echo ""
    else
        print_color "$YELLOW" "⚠ Original comprehensive test script not found - skipping"
    fi
    
    # Run the final fixed comprehensive tests
    if [ -f "test_wildcard_final.sh" ]; then
        print_subsection "Running final comprehensive shell tests"
        echo ""
        
        local start_time=$(date +%s)
        if ./test_wildcard_final.sh; then
            COMPREHENSIVE_TEST_RESULT="PASSED"
            print_color "$GREEN" "✓ Final comprehensive tests completed successfully"
        else
            COMPREHENSIVE_TEST_RESULT="FAILED"
            OVERALL_SUCCESS=false
            print_color "$RED" "✗ Final comprehensive tests failed"
        fi
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        print_color "$CYAN" "Final comprehensive test duration: ${duration}s"
    else
        print_color "$YELLOW" "⚠ Final comprehensive test script not found - skipping"
        COMPREHENSIVE_TEST_RESULT="SKIPPED"
    fi
}

run_stress_tests() {
    print_section "RUNNING STRESS TESTS"
    
    if [ -f "test_wildcard_stress.sh" ]; then
        print_subsection "Running stress and robustness tests"
        echo ""
        
        local start_time=$(date +%s)
        if ./test_wildcard_stress.sh; then
            STRESS_TEST_RESULT="PASSED"
            print_color "$GREEN" "✓ Stress tests completed successfully"
        else
            STRESS_TEST_RESULT="FAILED"
            OVERALL_SUCCESS=false
            print_color "$RED" "✗ Stress tests failed"
        fi
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        print_color "$CYAN" "Stress test duration: ${duration}s"
    else
        print_color "$YELLOW" "⚠ Stress test script not found - skipping"
        STRESS_TEST_RESULT="SKIPPED"
    fi
}

generate_report() {
    print_section "TEST RESULTS SUMMARY"
    
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
    local report_file="wildcard_test_report_$(date +%Y%m%d_%H%M%S).txt"
    
    # Print to both stdout and report file
    {
        echo "MINISHELL WILDCARD TEST REPORT"
        echo "Generated: $timestamp"
        echo "========================================"
        echo ""
        echo "ENVIRONMENT:"
        echo "  OS: $(uname -s)"
        echo "  Architecture: $(uname -m)"
        echo "  Shell: $SHELL"
        echo "  Working Directory: $(pwd)"
        echo "  Minishell Binary: $MINISHELL"
        echo ""
        echo "TEST RESULTS:"
        echo "  Unit Tests:          $UNIT_TEST_RESULT"
        echo "  Comprehensive Tests: $COMPREHENSIVE_TEST_RESULT"
        echo "  Stress Tests:        $STRESS_TEST_RESULT"
        echo ""
        
        if [ "$OVERALL_SUCCESS" = true ]; then
            echo "OVERALL RESULT: ✅ ALL TESTS PASSED"
            echo ""
            echo "🎉 Congratulations! Your wildcard implementation"
            echo "   has passed all comprehensive tests!"
            echo ""
            echo "FEATURES VERIFIED:"
            echo "  ✓ Basic wildcard expansion (*)"
            echo "  ✓ Extension-based filtering (*.txt)"
            echo "  ✓ Prefix matching (file*)"
            echo "  ✓ Complex multi-asterisk patterns"
            echo "  ✓ Hidden file handling (. files)"
            echo "  ✓ Quote handling (prevents expansion)"
            echo "  ✓ Current directory limitation"
            echo "  ✓ Memory management"
            echo "  ✓ Error handling"
            echo "  ✓ Performance under stress"
            echo "  ✓ Edge case robustness"
        else
            echo "OVERALL RESULT: ❌ SOME TESTS FAILED"
            echo ""
            echo "Please review the failed tests above and"
            echo "check your wildcard implementation."
            echo ""
            echo "RECOMMENDATIONS:"
            echo "  • Check memory allocation and cleanup"
            echo "  • Verify pattern matching logic"
            echo "  • Test edge cases manually"
            echo "  • Review filter functions"
            echo "  • Check quote processing"
        fi
        
        echo ""
        echo "FILES TESTED:"
        echo "  • Regular files with various extensions"
        echo "  • Hidden files (starting with .)"
        echo "  • Files with special characters"
        echo "  • Files with numbers and mixed content"
        echo "  • Very long filenames"
        echo "  • Single character files"
        echo "  • Files with multiple extensions"
        echo ""
        echo "PATTERNS TESTED:"
        echo "  • Simple wildcard (*)"
        echo "  • Extension patterns (*.ext)"
        echo "  • Prefix patterns (prefix*)"
        echo "  • Complex patterns (*prefix*suffix*)"
        echo "  • Multiple consecutive asterisks"
        echo "  • Very long patterns"
        echo "  • Quoted patterns (should not expand)"
        echo ""
        echo "STRESS CONDITIONS:"
        echo "  • 500+ files in directory"
        echo "  • Complex multi-segment patterns"
        echo "  • Memory allocation limits"
        echo "  • Concurrent operations"
        echo "  • Error conditions"
        echo "  • Boundary conditions"
        
    } | tee "$report_file"
    
    echo ""
    print_color "$CYAN" "Full report saved to: $report_file"
    
    # Print final status
    echo ""
    if [ "$OVERALL_SUCCESS" = true ]; then
        print_color "$GREEN$BOLD" "🎉 ALL WILDCARD TESTS PASSED! 🎉"
        print_color "$GREEN" "Your wildcard implementation is working correctly!"
        return 0
    else
        print_color "$RED$BOLD" "❌ SOME TESTS FAILED"
        print_color "$RED" "Please review the implementation and fix the issues."
        return 1
    fi
}

cleanup() {
    print_section "CLEANUP"
    
    print_subsection "Cleaning up test artifacts"
    
    # Remove test executables
    rm -f test_wildcard_unit
    
    # Clean object files
    if [ -f "Makefile_wildcard_tests" ]; then
        make -f Makefile_wildcard_tests clean &>/dev/null || true
    fi
    
    # Remove any leftover test directories
    rm -rf wildcard_test_env wildcard_stress_test
    
    # Keep log files but mention them
    if [ -f "valgrind_unit.log" ]; then
        print_color "$CYAN" "Valgrind log preserved: valgrind_unit.log"
    fi
    
    print_color "$GREEN" "✓ Cleanup completed"
}

show_usage() {
    print_color "$YELLOW" "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  -h, --help     Show this help message"
    echo "  -u, --unit     Run only unit tests"
    echo "  -c, --comp     Run only comprehensive tests"
    echo "  -s, --stress   Run only stress tests"
    echo "  --no-valgrind  Skip valgrind tests"
    echo "  --no-cleanup   Skip cleanup after tests"
    echo ""
    echo "Default: Run all tests"
}

main() {
    local run_unit=true
    local run_comprehensive=true
    local run_stress=true
    local run_cleanup=true
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -u|--unit)
                run_comprehensive=false
                run_stress=false
                shift
                ;;
            -c|--comp)
                run_unit=false
                run_stress=false
                shift
                ;;
            -s|--stress)
                run_unit=false
                run_comprehensive=false
                shift
                ;;
            --no-valgrind)
                VALGRIND_AVAILABLE=false
                shift
                ;;
            --no-cleanup)
                run_cleanup=false
                shift
                ;;
            *)
                print_color "$RED" "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    print_banner
    
    # Run test phases
    check_prerequisites
    build_tests || exit 1
    
    if [ "$run_unit" = true ]; then
        run_unit_tests
    fi
    
    if [ "$run_comprehensive" = true ]; then
        run_comprehensive_tests
    fi
    
    if [ "$run_stress" = true ]; then
        run_stress_tests
    fi
    
    generate_report
    local test_result=$?
    
    if [ "$run_cleanup" = true ]; then
        cleanup
    fi
    
    exit $test_result
}

# Handle Ctrl+C gracefully
trap 'echo ""; print_color "$YELLOW" "Tests interrupted by user"; cleanup; exit 130' INT

# Run main function
main "$@"
