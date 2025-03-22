#!/bin/bash

# Test binary
OUTPUT="./a.out"

echo "Running separator tests..."

run_test() {
    echo "Test: $1"
    echo -n "      "
    $OUTPUT "$1"
    echo "----------------------"
	echo ""
}

# Define test cases with adjacent separators
tests=(
    "echo kuzey|grep k"                    # Pipe without spaces
    "echo hello|wc -c"                     # Pipe to wc
    "echo test>output_file"                # Redirection without spaces
    "echo append>>output_file"             # Append redirection without spaces
    "cat<<EOF\nline1\nline2\nEOF"          # Here-document without spaces
    "echo 'hello world'|tr a-z A-Z"        # Pipe to tr
    "false||echo 'fallback'"               # OR without spaces
    "echo yes&&echo no"                    # AND without spaces
    "echo left<file"                       # Input redirection without spaces
    "echo write>>append_file"              # Append redirection
    "echo multiple|grep m|wc -c"           # Multiple pipes without spaces
    "echo mix&&echo pipe|wc -c"            # AND + Pipe combo
    "echo 'quotes test'|grep 'test'"       # Quoted argument with adjacent pipe
    "echo nested|(echo inner)|wc -c"       # Nested subshell with pipe
    "(echo multi<<EOF done) | wc -c"       # Here-document inside subshell
    "echo kuzey |grep k"
    "echo test> output_file"
    "false|| echo 'fallback'"
    "echo kuzey|grep k"
    "echo test>output_file"
    "false||echo 'fallback'"
    "echo kuzey | grep k"
    "echo test > output_file"
    "false || echo 'fallback'"
    "echo kuzey   |   grep k"
    "echo test     >      output_file"
    "false   ||   echo 'fallback'"
    "echo 'hello|world'"
    "echo \"this>should>not>redirect\""
    "echo 'fail||success'"
    "echo 'hello |world'"
    "echo 'hello | world'"
    "echo \"this > should > not > redirect\""
    "echo 'fail || success'"
    "echo 'hello |world'"
    "echo \"this> should>not >redirect\""
    "echo 'fail|| success'"
    "echo 'hello   |   world'"
    "echo \"this    >   should    >    not   >   redirect\""
    "echo 'fail   ||    success'"
)

# Run each test
for test in "${tests[@]}"; do
    run_test "$test"
done

echo "Tests completed!"