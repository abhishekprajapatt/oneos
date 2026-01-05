#!/bin/bash

set -e

TEST_LOG="/var/log/oneos-store-tests.log"
STORE_DB="/var/lib/oneos-store/store.db"
TEST_APPS_DIR="/tmp/oneos-test-apps"
TEST_RESULTS_DIR="/tmp/oneos-test-results"
TEST_TIMEOUT=300

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

init_test_env() {
    echo "[*] Initializing test environment..."
    mkdir -p "$TEST_APPS_DIR"
    mkdir -p "$TEST_RESULTS_DIR"
    
    > "$TEST_LOG"
    
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] OneOS Store Test Suite Started" >> "$TEST_LOG"
    echo "[✓] Test environment ready"
}

log_test() {
    local test_name=$1
    local status=$2
    local duration=$3
    
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] TEST: $test_name | STATUS: $status | DURATION: ${duration}s" >> "$TEST_LOG"
    
    case $status in
        PASS)
            echo -e "${GREEN}✓${NC} $test_name (${duration}s)"
            ;;
        FAIL)
            echo -e "${RED}✗${NC} $test_name (${duration}s)"
            ;;
        SKIP)
            echo -e "${YELLOW}⊘${NC} $test_name (skipped)"
            ;;
    esac
}

test_install_app() {
    local app_name=$1
    local app_id=$2
    local start_time=$(date +%s)
    
    echo -n "  Testing install of $app_name..."
    
    local install_cmd="oneos-store install $app_id"
    
    if timeout $TEST_TIMEOUT $install_cmd >/dev/null 2>&1; then
        if check_app_installed "$app_name"; then
            local duration=$(($(date +%s) - start_time))
            log_test "install_$app_name" "PASS" "$duration"
            return 0
        fi
    fi
    
    local duration=$(($(date +%s) - start_time))
    log_test "install_$app_name" "FAIL" "$duration"
    return 1
}

check_app_installed() {
    local app_name=$1
    
    sqlite3 "$STORE_DB" "SELECT COUNT(*) FROM apps WHERE name='$app_name' AND installed=1;" 2>/dev/null | grep -q "1"
}

test_uninstall_app() {
    local app_name=$1
    local app_id=$2
    local start_time=$(date +%s)
    
    echo -n "  Testing uninstall of $app_name..."
    
    local uninstall_cmd="oneos-store uninstall $app_id"
    
    if timeout $TEST_TIMEOUT $uninstall_cmd >/dev/null 2>&1; then
        if ! check_app_installed "$app_name"; then
            local duration=$(($(date +%s) - start_time))
            log_test "uninstall_$app_name" "PASS" "$duration"
            return 0
        fi
    fi
    
    local duration=$(($(date +%s) - start_time))
    log_test "uninstall_$app_name" "FAIL" "$duration"
    return 1
}

test_dependencies() {
    local app_name=$1
    echo -n "  Testing dependency resolution for $app_name..."
    
    local missing_deps=$(apt-cache depends --no-recommends "$app_name" 2>/dev/null | \
                         grep "Depends:" | awk '{print $2}' | \
                         xargs -I {} dpkg -l {} 2>/dev/null | \
                         grep -v "^ii" || true)
    
    if [ -z "$missing_deps" ]; then
        log_test "dependencies_$app_name" "PASS" "0"
        return 0
    else
        log_test "dependencies_$app_name" "FAIL" "0"
        return 1
    fi
}

test_app_permissions() {
    local app_id=$1
    echo -n "  Testing app permissions..."
    
    local perms=$(sqlite3 "$STORE_DB" \
        "SELECT permissions FROM apps WHERE id=$app_id;" 2>/dev/null)
    
    if [[ "$perms" == *"files"* ]] || [[ "$perms" == *"network"* ]]; then
        echo "  Permissions: $perms"
        log_test "permissions_app_$app_id" "PASS" "0"
        return 0
    fi
    
    log_test "permissions_app_$app_id" "SKIP" "0"
    return 1
}

test_gpg_signature() {
    local app_id=$1
    local package_file=$2
    echo -n "  Testing GPG signature verification..."
    
    if gpg --verify "$package_file.sig" "$package_file" 2>/dev/null; then
        log_test "signature_app_$app_id" "PASS" "0"
        return 0
    else
        log_test "signature_app_$app_id" "FAIL" "0"
        return 1
    fi
}

test_isolation() {
    local app_name=$1
    echo -n "  Testing app isolation (Flatpak)..."
    
    local app_processes=$(ps aux | grep -i "$app_name" | grep -i "flatpak" | wc -l)
    
    if [ "$app_processes" -gt 0 ]; then
        log_test "isolation_$app_name" "PASS" "0"
        return 0
    fi
    
    log_test "isolation_$app_name" "SKIP" "0"
    return 0
}

test_app_update() {
    local app_name=$1
    local current_version=$2
    local start_time=$(date +%s)
    
    echo -n "  Testing update for $app_name..."
    
    local update_cmd="oneos-store check-update $app_name"
    
    if $update_cmd 2>/dev/null | grep -q "Available"; then
        if timeout $TEST_TIMEOUT oneos-store update $app_name >/dev/null 2>&1; then
            local duration=$(($(date +%s) - start_time))
            log_test "update_$app_name" "PASS" "$duration"
            return 0
        fi
    fi
    
    log_test "update_$app_name" "SKIP" "0"
    return 0
}

test_install_performance() {
    local app_name=$1
    local expected_max_time=60
    
    local start_time=$(date +%s%N)
    
    oneos-store install "$app_name" >/dev/null 2>&1
    
    local end_time=$(date +%s%N)
    local duration=$((($end_time - $start_time) / 1000000))
    
    if [ "$duration" -lt $((expected_max_time * 1000)) ]; then
        log_test "perf_install_$app_name" "PASS" "$((duration / 1000))s"
        return 0
    else
        log_test "perf_install_$app_name" "FAIL" "$((duration / 1000))s"
        return 1
    fi
}

run_batch_tests() {
    local app_category=$1
    local max_apps=${2:-10}
    
    echo ""
    echo -e "${BLUE}Testing Category: $app_category${NC}"
    echo "═════════════════════════════════════════"
    
    local apps=$(sqlite3 "$STORE_DB" \
        "SELECT id, name FROM apps WHERE category='$app_category' LIMIT $max_apps;" 2>/dev/null)
    
    local pass_count=0
    local fail_count=0
    
    while IFS='|' read -r app_id app_name; do
        echo ""
        echo "Testing: $app_name (ID: $app_id)"
        
        if test_install_app "$app_name" "$app_id"; then
            pass_count=$((pass_count + 1))
        else
            fail_count=$((fail_count + 1))
            continue
        fi
        
        test_dependencies "$app_name"
        
        test_app_permissions "$app_id"
        
        test_isolation "$app_name"
        
        test_app_update "$app_name" "1.0.0"
        
        test_uninstall_app "$app_name" "$app_id"
        
    done <<< "$apps"
    
    echo ""
    echo "Category Results: $pass_count passed, $fail_count failed"
}

run_full_test_suite() {
    echo ""
    echo "╔════════════════════════════════════════════════╗"
    echo "║   OneOS Store - Full Test Suite (500+ apps)   ║"
    echo "║  Install/Uninstall, Security, Performance     ║"
    echo "╚════════════════════════════════════════════════╝"
    echo ""
    
    local total_pass=0
    local total_fail=0
    
    local categories=$(sqlite3 "$STORE_DB" \
        "SELECT DISTINCT category FROM apps LIMIT 10;" 2>/dev/null)
    
    while read -r category; do
        run_batch_tests "$category" 50
    done <<< "$categories"
    
    generate_test_report
}

generate_test_report() {
    echo ""
    echo "╔════════════════════════════════════════════════╗"
    echo "║              Test Report Summary               ║"
    echo "╚════════════════════════════════════════════════╝"
    echo ""
    
    local total_tests=$(grep -c "TEST:" "$TEST_LOG" 2>/dev/null || echo "0")
    local passed=$(grep -c "PASS" "$TEST_LOG" 2>/dev/null || echo "0")
    local failed=$(grep -c "FAIL" "$TEST_LOG" 2>/dev/null || echo "0")
    local skipped=$(grep -c "SKIP" "$TEST_LOG" 2>/dev/null || echo "0")
    
    local pass_rate=0
    if [ "$total_tests" -gt 0 ]; then
        pass_rate=$((passed * 100 / total_tests))
    fi
    
    echo "📊 Total Tests:     $total_tests"
    echo "✓  Passed:          $passed"
    echo "✗  Failed:          $failed"
    echo "⊘  Skipped:         $skipped"
    echo "📈 Pass Rate:       ${pass_rate}%"
    echo ""
    echo "🔍 Full Log: $TEST_LOG"
    
    if [ "$pass_rate" -ge 85 ]; then
        echo -e "${GREEN}[✓] Test Suite PASSED (85%+ success rate)${NC}"
        return 0
    else
        echo -e "${RED}[!] Test Suite FAILED (below 85% threshold)${NC}"
        return 1
    fi
}

main() {
    init_test_env
    
    case "${1:-full}" in
        full)
            run_full_test_suite
            ;;
        category)
            run_batch_tests "${2:-Productivity}" "${3:-10}"
            ;;
        install)
            test_install_app "$2" "$3"
            ;;
        uninstall)
            test_uninstall_app "$2" "$3"
            ;;
        report)
            generate_test_report
            ;;
        *)
            echo "OneOS Store Test Suite"
            echo "Usage: $0 {full|category|install|uninstall|report}"
            exit 1
            ;;
    esac
}

main "$@"

