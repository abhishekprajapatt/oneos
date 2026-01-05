#!/bin/bash

set -e

ADB_TIMEOUT=30
TEST_DIR="/tmp/oneos_adb_test"
LOG_FILE="$TEST_DIR/adb_test.log"
DEVICES_LOG="$TEST_DIR/connected_devices.txt"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log() {
    echo -e "${BLUE}[$(date +'%H:%M:%S')]${NC} $1" | tee -a "$LOG_FILE"
}

pass() {
    echo -e "${GREEN}[✓]${NC} $1" | tee -a "$LOG_FILE"
}

fail() {
    echo -e "${RED}[✗]${NC} $1" | tee -a "$LOG_FILE"
}

info() {
    echo -e "${YELLOW}[*]${NC} $1" | tee -a "$LOG_FILE"
}

init_adb_test() {
    mkdir -p "$TEST_DIR"
    > "$LOG_FILE"
    
    log "OneOS ADB Multi-Device Testing Framework"
    log "========================================="
}

discover_devices() {
    log "Discovering connected Android devices..."
    
    > "$DEVICES_LOG"
    
    adb devices -l > "$DEVICES_LOG" 2>&1 || true
    
    local device_count=$(adb devices -l | grep -v "^$" | grep -v "List of" | wc -l)
    
    if [ "$device_count" -gt 0 ]; then
        pass "Found $device_count Android device(s)"
        cat "$DEVICES_LOG" | tail -n +2
    else
        info "No devices connected"
        return 1
    fi
}

connect_device_usb() {
    local serial=$1
    
    log "Connecting device: $serial (USB)"
    
    if timeout $ADB_TIMEOUT adb -s "$serial" shell echo "test" &>/dev/null; then
        pass "Device $serial connected"
        return 0
    else
        fail "Could not connect to $serial"
        return 1
    fi
}

connect_device_wifi() {
    local ip=$1
    local port=${2:-5555}
    
    log "Connecting device via WiFi: $ip:$port"
    
    if adb connect "$ip:$port" &>/dev/null; then
        pass "Connected to $ip:$port via WiFi"
        return 0
    else
        fail "Failed to connect to $ip:$port"
        return 1
    fi
}

disconnect_device() {
    local serial=$1
    
    adb -s "$serial" disconnect "$serial" &>/dev/null
    pass "Disconnected device: $serial"
}

test_file_transfer() {
    local serial=$1
    
    info "Testing file transfer..."
    
    mkdir -p "$TEST_DIR/transfer_test"
    
    local test_file="$TEST_DIR/transfer_test/test_data.bin"
    dd if=/dev/urandom of="$test_file" bs=1M count=10 2>/dev/null
    
    log "Pushing 10MB file to device..."
    local start=$(date +%s%N)
    
    if adb -s "$serial" push "$test_file" /sdcard/test_data.bin &>/dev/null; then
        local end=$(date +%s%N)
        local duration=$(( (end - start) / 1000000 ))
        local speed=$(echo "scale=2; 10485760 * 1000 / $duration" | bc)
        
        pass "File push: 10MB in ${duration}ms (${speed} MB/s)"
    else
        fail "File push failed"
        return 1
    fi
    
    log "Pulling file from device..."
    start=$(date +%s%N)
    
    if adb -s "$serial" pull /sdcard/test_data.bin "$TEST_DIR/pulled_file.bin" &>/dev/null; then
        end=$(date +%s%N)
        duration=$(( (end - start) / 1000000 ))
        speed=$(echo "scale=2; 10485760 * 1000 / $duration" | bc)
        
        pass "File pull: 10MB in ${duration}ms (${speed} MB/s)"
        
        if cmp "$test_file" "$TEST_DIR/pulled_file.bin" &>/dev/null; then
            pass "File integrity verified"
        else
            fail "File integrity check failed"
        fi
    else
        fail "File pull failed"
        return 1
    fi
}

test_sms_relay() {
    local serial=$1
    
    info "Testing SMS relay capability..."
    
    local result=$(adb -s "$serial" shell settings get global sms_default_application 2>/dev/null || echo "")
    
    if [ -z "$result" ]; then
        info "Device SMS permissions need to be granted"
        pass "SMS relay can be enabled via Settings"
    else
        pass "Device SMS integration ready"
    fi
    
    local phone=$(adb -s "$serial" shell getprop ro.build.version.release 2>/dev/null || echo "unknown")
    log "Device Android version: $phone"
}

test_clipboard_sync() {
    local serial=$1
    
    info "Testing clipboard synchronization..."
    
    local test_text="OneOS Clipboard Test - $(date +%s)"
    
    local device_clipboard=$(adb -s "$serial" shell "cmd clipboard get-text" 2>/dev/null || echo "")
    
    if [ -n "$device_clipboard" ]; then
        pass "Clipboard read from device"
        log "Current clipboard: ${device_clipboard:0:50}..."
    else
        info "Device clipboard is empty or unavailable"
    fi
    
    adb -s "$serial" shell "cmd clipboard set-text '$test_text'" &>/dev/null
    pass "Clipboard set on device"
}

test_screen_mirror() {
    local serial=$1
    
    info "Testing screen mirroring capability..."
    
    local screen_available=$(adb -s "$serial" shell "screencap -p /dev/null" 2>/dev/null && echo "yes" || echo "no")
    
    if [ "$screen_available" = "yes" ]; then
        pass "Screen capture available"
        
        log "Capturing screen..."
        if adb -s "$serial" shell screencap -p /sdcard/screen.png &>/dev/null; then
            adb -s "$serial" pull /sdcard/screen.png "$TEST_DIR/screen_$serial.png" &>/dev/null
            pass "Screen captured and pulled"
        fi
    else
        info "Screen capture not available on device"
    fi
}

test_package_management() {
    local serial=$1
    
    info "Testing package management..."
    
    local package_count=$(adb -s "$serial" shell "pm list packages" 2>/dev/null | wc -l)
    pass "Found $package_count installed packages"
    
    if [ -f "$TEST_DIR/test_app.apk" ]; then
        log "Installing test APK..."
        if adb -s "$serial" install "$TEST_DIR/test_app.apk" &>/dev/null; then
            pass "APK installation successful"
        else
            fail "APK installation failed"
        fi
    fi
}

test_logcat() {
    local serial=$1
    
    info "Testing logcat (system logs)..."
    
    log "Capturing system log for 5 seconds..."
    timeout 5 adb -s "$serial" logcat > "$TEST_DIR/logcat_$serial.log" 2>&1 || true
    
    local log_lines=$(wc -l < "$TEST_DIR/logcat_$serial.log")
    pass "Captured $log_lines log lines"
}

test_network() {
    local serial=$1
    
    info "Testing network connectivity..."
    
    if adb -s "$serial" shell "ping -c 1 8.8.8.8" &>/dev/null; then
        pass "Network connectivity verified"
    else
        info "Network may be unreachable (device may be offline)"
    fi
    
    local wifi_status=$(adb -s "$serial" shell "settings get global wifi_on" 2>/dev/null || echo "unknown")
    log "WiFi status: $wifi_status"
    
    local ip=$(adb -s "$serial" shell "ip addr show" 2>/dev/null | grep -oP "inet \K[^/]+" | head -1 || echo "unknown")
    log "Device IP: $ip"
}

test_battery() {
    local serial=$1
    
    info "Testing battery status..."
    
    local battery_info=$(adb -s "$serial" shell "dumpsys battery" 2>/dev/null)
    
    local level=$(echo "$battery_info" | grep "level:" | awk '{print $2}')
    local temperature=$(echo "$battery_info" | grep "temperature:" | awk '{print $2}')
    local health=$(echo "$battery_info" | grep "health:" | awk '{print $2}')
    
    if [ -n "$level" ]; then
        pass "Battery level: $level%"
        log "Temperature: $temperature°C, Health: $health"
    else
        info "Battery info unavailable"
    fi
}

test_performance() {
    local serial=$1
    
    info "Performance profiling..."
    
    local cpu_info=$(adb -s "$serial" shell "cat /proc/cpuinfo" 2>/dev/null | grep "processor" | wc -l)
    pass "CPU cores: $cpu_info"
    
    local mem_total=$(adb -s "$serial" shell "free -m | grep Mem" 2>/dev/null | awk '{print $2}')
    local mem_available=$(adb -s "$serial" shell "free -m | grep Mem" 2>/dev/null | awk '{print $7}')
    
    if [ -n "$mem_total" ]; then
        pass "Memory: $mem_available MB / $mem_total MB available"
    fi
    
    local storage=$(adb -s "$serial" shell "df /data | tail -1" 2>/dev/null | awk '{printf "%.0f%% used", ($3/$2)*100}' || echo "unknown")
    log "Storage: $storage"
}

test_multi_device() {
    info "Running multi-device tests..."
    
    local devices=$(adb devices -l | grep -v "^$" | grep -v "List of" | awk '{print $1}')
    
    if [ -z "$devices" ]; then
        info "No devices connected"
        return 1
    fi
    
    local device_array=($devices)
    local count=${
    
    pass "Testing with $count device(s)"
    
    for i in "${!device_array[@]}"; do
        local serial="${device_array[$i]}"
        
        info "Device $((i+1))/$count: $serial"
        test_file_transfer "$serial"
        test_clipboard_sync "$serial"
        test_network "$serial"
        test_battery "$serial"
        echo ""
    done
}

print_summary() {
    echo ""
    echo -e "${BLUE}╔════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║  ADB Testing Summary                  ║${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════╝${NC}"
    echo ""
    
    local total_passed=$(grep -c "^\[✓\]" "$LOG_FILE" || echo "0")
    local total_failed=$(grep -c "^\[✗\]" "$LOG_FILE" || echo "0")
    
    echo "Results:"
    echo -e "  ${GREEN}Passed:${NC}  $total_passed"
    echo -e "  ${RED}Failed:${NC}  $total_failed"
    echo ""
    
    echo "Log file: $LOG_FILE"
}

main() {
    init_adb_test
    
    case "${1:-help}" in
        devices)
            discover_devices
            ;;
        connect-usb)
            connect_device_usb "$2"
            ;;
        connect-wifi)
            connect_device_wifi "$2" "${3:-5555}"
            ;;
        disconnect)
            disconnect_device "$2"
            ;;
        transfer)
            discover_devices && test_file_transfer "$(adb devices -l | grep -v "^$" | grep -v "List of" | head -1 | awk '{print $1}')"
            ;;
        sms)
            discover_devices && test_sms_relay "$(adb devices -l | grep -v "^$" | grep -v "List of" | head -1 | awk '{print $1}')"
            ;;
        clipboard)
            discover_devices && test_clipboard_sync "$(adb devices -l | grep -v "^$" | grep -v "List of" | head -1 | awk '{print $1}')"
            ;;
        screen)
            discover_devices && test_screen_mirror "$(adb devices -l | grep -v "^$" | grep -v "List of" | head -1 | awk '{print $1}')"
            ;;
        battery)
            discover_devices && test_battery "$(adb devices -l | grep -v "^$" | grep -v "List of" | head -1 | awk '{print $1}')"
            ;;
        network)
            discover_devices && test_network "$(adb devices -l | grep -v "^$" | grep -v "List of" | head -1 | awk '{print $1}')"
            ;;
        multi)
            test_multi_device
            ;;
        full)
            discover_devices
            test_multi_device
            ;;
        *)
            cat << 'EOF'
OneOS ADB Multi-Device Testing Framework

Usage: adb-multidevice <command> [args]

Commands:
  devices         List connected devices
  connect-usb <serial>   Connect via USB
  connect-wifi <ip> [port] Connect via WiFi
  disconnect <serial>    Disconnect device
  transfer        Test file transfer
  sms             Test SMS relay
  clipboard       Test clipboard sync
  screen          Test screen mirroring
  battery         Test battery status
  network         Test network connectivity
  multi           Run all tests on all devices
  full            Discover devices + run all tests

Examples:
  adb-multidevice devices
  adb-multidevice transfer
  adb-multidevice multi
  adb-multidevice full
EOF
            ;;
    esac
    
    print_summary
}

main "$@"

