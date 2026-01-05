#!/bin/bash

echo "=== OneOS /var System Test Suite ==="
echo ""

TESTS_PASSED=0
TESTS_FAILED=0

test_directory_exists() {
    local dir=$1
    if [ -d "$dir" ]; then
        echo "[+] Directory exists: $dir"
        ((TESTS_PASSED++))
    else
        echo "[-] Directory missing: $dir"
        ((TESTS_FAILED++))
    fi
}

test_file_exists() {
    local file=$1
    if [ -f "$file" ]; then
        echo "[+] File exists: $file"
        ((TESTS_PASSED++))
    else
        echo "[-] File missing: $file"
        ((TESTS_FAILED++))
    fi
}

test_executable() {
    local file=$1
    if [ -x "$file" ]; then
        echo "[+] Executable: $file"
        ((TESTS_PASSED++))
    else
        echo "[-] Not executable: $file"
        ((TESTS_FAILED++))
    fi
}

test_permissions() {
    local file=$1
    local expected=$2
    local actual=$(stat -c %a "$file" 2>/dev/null || stat -f %A "$file" 2>/dev/null)
    
    if [ "$actual" = "$expected" ]; then
        echo "[+] Permissions correct: $file ($expected)"
        ((TESTS_PASSED++))
    else
        echo "[-] Permissions wrong: $file (expected $expected, got $actual)"
        ((TESTS_FAILED++))
    fi
}

echo "[*] Testing directory structure..."
test_directory_exists "/var"
test_directory_exists "/var/cache"
test_directory_exists "/var/log"
test_directory_exists "/var/run"
test_directory_exists "/var/lib"
test_directory_exists "/var/spool"
test_directory_exists "/var/tmp"
test_directory_exists "/var/backups"
test_directory_exists "/var/crash"

echo ""
echo "[*] Testing subdirectories..."
test_directory_exists "/var/cache/apt"
test_directory_exists "/var/log/applications"
test_directory_exists "/var/run/pid"
test_directory_exists "/var/lib/dpkg"
test_directory_exists "/var/lib/rpm"
test_directory_exists "/var/spool/cron"

echo ""
echo "[*] Testing files..."
test_file_exists "/var/var_system.h"
test_file_exists "/var/var_system.c"
test_file_exists "/var/var_config.h"
test_file_exists "/var/build.sh"

echo ""
echo "[*] Summary:"
echo "    Passed: $TESTS_PASSED"
echo "    Failed: $TESTS_FAILED"
echo ""

if [ $TESTS_FAILED -eq 0 ]; then
    echo "[+] All tests passed!"
    exit 0
else
    echo "[-] Some tests failed"
    exit 1
fi

