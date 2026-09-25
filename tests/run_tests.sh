#!/usr/bin/env bash
set -u
COMPILER="${1:-./sadhu_bangla_compiler}"
PASS=0
FAIL=0

tmpdir=$(mktemp -d)
trap 'rm -rf "$tmpdir"' EXIT

run_success() {
    local src="$1"
    local out="$tmpdir/$(basename "$src" .sbb).py"
    if "$COMPILER" "$src" "$out" --no-run >"$tmpdir/stdout" 2>"$tmpdir/stderr" \
       && python3 -m py_compile "$out" >/dev/null 2>&1; then
        echo "PASS  $src"
        PASS=$((PASS + 1))
    else
        echo "FAIL  $src"
        cat "$tmpdir/stderr"
        FAIL=$((FAIL + 1))
    fi
}

run_failure() {
    local src="$1"
    if "$COMPILER" "$src" "$tmpdir/invalid.py" --no-run >"$tmpdir/stdout" 2>"$tmpdir/stderr"; then
        echo "FAIL  $src (expected compiler error)"
        FAIL=$((FAIL + 1))
    else
        echo "PASS  $src (rejected safely)"
        PASS=$((PASS + 1))
    fi
}

run_success tests/valid.sbb
run_success tests/division_ok.sbb
run_success tests/leading_zero.sbb
run_success tests/python_keyword.sbb
run_success tests/empty.sbb
run_failure tests/type_errors.sbb
run_failure tests/division_type_error.sbb
run_failure tests/missing_semicolon.sbb
run_failure tests/missing_brace.sbb
run_failure tests/bad_expr.sbb
run_failure tests/multi_errors.sbb
run_failure tests/invalid_loop_control.sbb

run_runtime() {
    local src="$1"
    local out="$tmpdir/runtime.py"
    local expected="$tmpdir/expected.txt"
    local actual="$tmpdir/actual.txt"

    cat >"$expected" <<'EOF_EXPECTED'
Shadharoner Tulonay Bhalo
1
3
4
6
EOF_EXPECTED

    if "$COMPILER" "$src" "$out" --no-run >"$tmpdir/runtime-compiler.stdout" 2>"$tmpdir/runtime-compiler.stderr" \
       && python3 "$out" >"$actual" 2>"$tmpdir/runtime-python.stderr" \
       && diff -u "$expected" "$actual" >/dev/null; then
        echo "PASS  $src (runtime output)"
        PASS=$((PASS + 1))
    else
        echo "FAIL  $src (runtime output)"
        cat "$tmpdir/runtime-compiler.stderr"
        cat "$tmpdir/runtime-python.stderr"
        cat "$actual" 2>/dev/null || true
        FAIL=$((FAIL + 1))
    fi
}

run_runtime examples/demo.sbb

echo
printf 'Passed: %d\nFailed: %d\n' "$PASS" "$FAIL"
test "$FAIL" -eq 0
