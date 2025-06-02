#!/bin/bash

# === Configuration ===
PARSER_EXEC="./minishell"  # ← Change to your actual parser binary
PROMPT_REGEX='^minishell *>?'


pass=0
fail=0
test_index=1

# Run a test case
test_case() {
    local raw_input="$1"
    local desc="$2"

    echo "Test $test_index: $desc"
    echo "Expression: $raw_input"

    # Convert raw expression to echo version for Bash
    local bash_expr
    bash_expr=$(echo "$raw_input" | sed -E 's/([0-9]+)/echo \1/g')

    # Bash output
    bash_output=$(bash -c "$bash_expr" 2>&1)

    # Parser output
    parser_output=$(echo "$raw_input" | "$PARSER_EXEC" 2>&1)

    # Remove any lines starting with "minishell >" or "minishell>"
    parser_output_cleaned=$(echo "$parser_output" | grep -vE "$PROMPT_REGEX")

    # Compare
    if [ "$bash_output" = "$parser_output_cleaned" ]; then
        echo "✅ PASS"
        ((pass++))
    else
        echo "❌ FAIL"
        echo "--- Bash output ---"
        echo "$bash_output"
        echo "--- Minishell output ---"
        echo "$parser_output_cleaned"
        ((fail++))
    fi

    echo
    ((test_index++))
}

# ========== BASIC TEST CASES ==========

test_case "1&&2" "Simple AND"
test_case "1||2" "Simple OR"

# ========== SEQUENCED OPERATORS ==========

test_case "1&&2||3" "AND followed by OR"
test_case "1||2&&3" "OR followed by AND"
test_case "1&&2&&3" "Chained ANDs"
test_case "1||2||3" "Chained ORs"

# ========== MIXED & COMPLEX EXPRESSIONS ==========

test_case "1&&2||3&&4" "Mix of AND and OR"
test_case "1||2&&3||4" "OR AND OR"
test_case "1&&2||3||4" "AND then OR chain"
test_case "1||2||3&&4&&5" "OR chain with final ANDs"
test_case "1&&2&&3||4||5" "Three ANDs followed by two ORs"

# ========== EDGE CASES ==========

test_case "1" "Single command"
test_case "1||2&&3&&4||5&&6" "Complex short-circuit behavior"
test_case "1&&2||3&&4||5" "Balanced mixed logic"
test_case "1&&2&&3&&4" "Long AND chain"
test_case "1||2||3||4" "Long OR chain"

# ========== WHITESPACE CASES ==========

test_case "  1  &&  2 " "AND with extra spaces"
test_case "1   ||    2" "OR with extra spaces"
test_case "  1  && 2 ||  3 " "AND-OR with mixed spacing"

# ========== KUZEY'S EXTRAS ==========
test_case "1&&(2||3)" "AND with OR in parentheses"
test_case "(1&&2)||3" "Parenthesized AND followed by OR"
test_case "1||(2&&3)" "OR with AND in parentheses"
test_case "(1||2)&&(3||4)" "Multiple ORs in parentheses combined with AND"
test_case "1&&((2||3)&&4)" "Nested parentheses"
test_case "((1&&2)||3)&&4" "Parentheses on both sides"
test_case "(((1))))" "Unbalanced parentheses (expect your parser to error)"
test_case "(1&&2)||(3&&(4||5))" "Balanced deep logic"
test_case "(1||2)||(3&&4)||5" "Long OR chain in parentheses"
test_case "((1&&2)&&3)||(4&&(5||6))" "Complex nested logic"
test_case "1&&2&&3&&4&&5&&6&&7" "Long AND chain"
test_case "1||2||3||4||5||6||7" "Long OR chain"
test_case "1&&2||3&&4||5&&6||7" "Alternating chain"
test_case "((((1&&2)||3)&&4)||5)&&6" "Nested and layered logic"
test_case "1&&(2&&(3&&(4&&(5&&6))))" "Deep AND nesting"
test_case "(1||2)&&(3||4)&&(5||6)" "Sequential parenthesized ORs"
test_case "(1&&(2||3))||4" "Mixed levels"
test_case "((1&&2)||(3||4))&&5" "Dual nesting"
test_case "(1&&(2&&(3||(4&&5))))" "Hybrid nested logic"
test_case "(1)||(2||(3||(4||(5))))" "Deep OR nesting"
test_case "(1&&2)||(3&&4)||(5&&6)" "Balanced complex expression"
test_case "1&&2&&(3||4||5)&&6&&7" "Complex AND with OR middle"
test_case "((1&&2&&3)||(4&&5&&6))&&(7||8||9)" "Balanced performance test"
test_case "(1)||((2)&&((3)||(4)))" "Multi-layer group"
test_case "1&&(2||3)&&4&&(5||6)&&7" "Alternating nested structure"
test_case "1&&(2||3||(4&&5||(6&&7)))" "Complex nested fallback logic"
test_case "((((1||2)&&3)||4)&&5)||(6&&7)" "Stress test for parser"
test_case "1||(2&&(3||(4&&(5||(6&&(7))))))" "Deep hybrid nest"
test_case "1&&(2||(3&&4&&5)&&7||8)||9&&10" "random stuff"
test_case "1 && (2 ||( 3&&  4&&  5) &&7  ||8)   ||9  && 10" "random stuff"


# ========== SUMMARY ==========

echo "========================="
echo "Tests passed: $pass"
echo "Tests failed: $fail"
