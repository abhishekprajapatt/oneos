#!/bin/bash

CRASH_DIR="/var/crash"

save_crash() {
    local app_name=$1
    local pid=$2
    local signal=$3
    
    local timestamp=$(date +%Y%m%d_%H%M%S)
    local crash_file="$CRASH_DIR/${app_name}_${pid}_${timestamp}.crash"
    
    {
        echo "=== Crash Report ==="
        echo "Application: $app_name"
        echo "PID: $pid"
        echo "Signal: $signal"
        echo "Timestamp: $timestamp"
        echo "---"
        echo "System Information:"
        uname -a
        echo "---"
        echo "Memory Info:"
        free -h
    } > "$crash_file"
    
    echo "Crash saved: $crash_file"
}

list_crashes() {
    echo "=== Crash Reports ==="
    ls -lh "$CRASH_DIR"/*.crash 2>/dev/null || echo "No crash reports found"
}

analyze_crash() {
    local crash_file=$1
    
    if [ ! -f "$crash_file" ]; then
        echo "Crash file not found: $crash_file"
        return 1
    fi
    
    echo "=== Crash Analysis ==="
    cat "$crash_file"
}

cleanup_old_crashes() {
    local days=${1:-7}
    find "$CRASH_DIR" -name "*.crash" -mtime +$days -delete
    echo "Removed crash reports older than $days days"
}

crash_count() {
    find "$CRASH_DIR" -name "*.crash" -type f | wc -l
}

case "$1" in
    save)
        save_crash "$2" "$3" "$4"
        ;;
    list)
        list_crashes
        ;;
    analyze)
        analyze_crash "$2"
        ;;
    cleanup)
        cleanup_old_crashes "$2"
        ;;
    count)
        crash_count
        ;;
    *)
        echo "Usage: $0 {save <app> <pid> <signal>|list|analyze <file>|cleanup [days]|count}"
        ;;
esac

