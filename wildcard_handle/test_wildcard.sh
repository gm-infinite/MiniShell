#!/bin/bash

# Directory for testing
TEST_DIR="test_env"
mkdir -p "$TEST_DIR"

# Files to create for testing, including hidden files
TEST_FILES=(
  "apple.txt"
  "apricot.txt"
  "banana.txt"
  "a.out"
  "README.md"
  "Makefile"
  "main.c"
  ".hidden"
  ".secret"
  ".config_file"
  "test1.txt"
  "test2.txt"
  "alpha.txt"
  "beta.txt"
  "a_apple.txt"
  "foo.bar"
)

# Wildcard patterns to test (only containing at least one `*`)
PATTERNS=(
  "*"
  "a*"
  "*e*"
  "*E*"
  "*.txt"
  "*.*"
  "a*.*"
  "*o*"
  "*c"
  "*.c"
  "*.txt"
  "*alpha*"
  "*.*txt"
  "*.sh"
  "*secret*"
  "*config*"
  ".*"
)

# Create test files
cd "$TEST_DIR"
for f in "${TEST_FILES[@]}"; do
    touch "$f"
done
cd ..

# Compare output from Bash and your wildcard_handle
function expect_match() {
    local pattern="$1"

    pushd "$TEST_DIR" > /dev/null

    # Bash expansion output (simulate what shell sees)
    BASH_RESULT=$(compgen -G "$pattern" | sort)

    # Output from your program
    YOUR_RESULT=$(../a.out "$pattern" | tr ' ' '\n' | sort)

    # Display expected and actual results
    echo "Testing pattern: '$pattern'"
    echo "  Expected (bash):"
    echo "$BASH_RESULT"
    echo "  Got (your program):"
    echo "$YOUR_RESULT"

    if diff <(echo "$BASH_RESULT") <(echo "$YOUR_RESULT") > /dev/null; then
        echo "[PASS] '$pattern'"
    else
        echo "[FAIL] '$pattern'"
    fi
    echo "--------------------------------------------"

    popd > /dev/null
}

# Run all test patterns
for pat in "${PATTERNS[@]}"; do
    expect_match "$pat"
done

# Clean up
rm -rf "$TEST_DIR"
