#!/usr/bin/env bash
# End-to-end smoke test for the TaskFlow console application.
# Usage: bash tests/smoke_test.sh ./taskflow
set -euo pipefail

BIN="${1:-./taskflow}"

if [[ ! -x "$BIN" ]]; then
    echo "Usage: $0 <path-to-taskflow-binary>" >&2
    exit 2
fi

run_case() {
    local name="$1"
    local input="$2"
    shift 2

    local output
    local -a command=("$BIN")
    if command -v timeout >/dev/null 2>&1; then
        command=(timeout 15s "$BIN")
    fi

    if ! output=$(printf '%s' "$input" | TERM=xterm "${command[@]}" 2>&1); then
        echo "FAIL: $name exited with a non-zero status" >&2
        printf '%s\n' "$output" >&2
        exit 1
    fi

    local expected
    for expected in "$@"; do
        if ! grep -Fq -- "$expected" <<<"$output"; then
            echo "FAIL: $name did not contain: $expected" >&2
            printf '%s\n' "$output" >&2
            exit 1
        fi
    done

    echo "PASS: $name"
}

run_case "quit" $'0\n' \
    "TASKFLOW" \
    "THANK YOU FOR USING TASKFLOW"

run_case "full-flow" $'1\nWrite smoke test\n\n2\n\n3\n\n1\n\n4\n\n2\n\n0\n' \
    "Task added successfully" \
    "Write smoke test" \
    "Completed: 1" \
    "Removed 1 completed task" \
    "THANK YOU"

run_case "invalid-input" $'not-a-number\n\n0\n' \
    "Invalid input" \
    "THANK YOU"

echo "All smoke tests passed."
