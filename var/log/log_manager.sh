#!/bin/bash

LOG_DIR="/var/log"
RETENTION_DAYS=30
MAX_SIZE=104857600

rotate_logs() {
    local log_file=$1
    
    if [ ! -f "$log_file" ]; then
        return
    fi
    
    local file_size=$(stat -f%z "$log_file" 2>/dev/null || stat -c%s "$log_file" 2>/dev/null)
    
    if [ "$file_size" -gt "$MAX_SIZE" ]; then
        local timestamp=$(date +%Y%m%d_%H%M%S)
        mv "$log_file" "$log_file.$timestamp"
        gzip "$log_file.$timestamp"
        touch "$log_file"
    fi
}

cleanup_old_logs() {
    find "$LOG_DIR" -name "*.gz" -mtime +$RETENTION_DAYS -delete
    find "$LOG_DIR" -name "*.old" -mtime +$RETENTION_DAYS -delete
}

init_logs() {
    touch "$LOG_DIR/syslog"
    touch "$LOG_DIR/auth.log"
    touch "$LOG_DIR/kernel.log"
    touch "$LOG_DIR/messages"
    chmod 640 "$LOG_DIR"/*
}

rotate_all_logs() {
    for log_file in "$LOG_DIR"/*.log "$LOG_DIR"/syslog "$LOG_DIR"/messages; do
        [ -f "$log_file" ] && rotate_logs "$log_file"
    done
    cleanup_old_logs
}

view_logs() {
    local count=${1:-50}
    tail -n "$count" "$LOG_DIR/syslog"
}

case "$1" in
    init)
        init_logs
        echo "Initialized logging"
        ;;
    rotate)
        rotate_all_logs
        echo "Rotated logs"
        ;;
    view)
        view_logs "$2"
        ;;
    *)
        echo "Usage: $0 {init|rotate|view [count]}"
        exit 1
        ;;
esac

