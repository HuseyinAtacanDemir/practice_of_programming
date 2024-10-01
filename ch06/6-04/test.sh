#!/bin/bash

# Test helper function to log and run the command
run_test() {
    local desc="$1"
    local cmd="$2"
    echo "=========================================="
    echo "Test: $desc"
    echo "Command: $cmd"
    echo "------------------------------------------"
    $cmd
    echo "------------------------------------------"
    echo
}

# Path to your freq executable
FREQ_EXEC="./freq"

# Begin Tests

# Simple Tests
run_test "Help option" "$FREQ_EXEC -h"

run_test "Raw option" "$FREQ_EXEC -R"

run_test "Sort option" "$FREQ_EXEC -s"

run_test "Int option" "$FREQ_EXEC -i"

run_test "Double option" "$FREQ_EXEC -d"

run_test "Float option" "$FREQ_EXEC -f"

run_test "Long option" "$FREQ_EXEC -l"

# Test delimiter option with argument
run_test "Delimiter option with space delimiter" "$FREQ_EXEC -D ' '"

run_test "Delimiter option with comma delimiter" "$FREQ_EXEC -D ','"

# Combination Tests (Simple Combinations)
run_test "Raw and Sort options" "$FREQ_EXEC -R -s"

run_test "Raw, Int, and Delim options" "$FREQ_EXEC -R -i -D ':'"

run_test "Float and Long options" "$FREQ_EXEC -f -l"

# Complex Combinations (Multiple Options)
run_test "Raw, Sort, and Delimiter with comma" "$FREQ_EXEC -R -s -D ','"

run_test "Int, Double, Sort, and Delimiter with colon" "$FREQ_EXEC -i -d -s -D ':'"

run_test "All Data Type Options (Mutually Exclusive Check)" "$FREQ_EXEC -i -d -f -l"

# Invalid Tests (Should Fail)
run_test "Invalid option (should fail)" "$FREQ_EXEC -z"

run_test "Missing argument for delimiter" "$FREQ_EXEC -D"

run_test "Mutually exclusive options Int and Double" "$FREQ_EXEC -i -d"

run_test "Mutually exclusive options Float and Long" "$FREQ_EXEC -f -l"

run_test "Mutually exclusive options Int and Long" "$FREQ_EXEC -i -l"

# Edge Case: No options or arguments
run_test "No options or arguments" "$FREQ_EXEC"

# Edge Case: Delimiter but no argument
run_test "Delimiter option without argument" "$FREQ_EXEC -D"

# End of Tests
echo "All tests completed."

