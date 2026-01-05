#!/bin/bash

RUN_DIR="/var/run"

create_pid_file() {
    local service_name=$1
    local pid=$2
    
    echo "$pid" > "$RUN_DIR/pid/$service_name.pid"
    chmod 644 "$RUN_DIR/pid/$service_name.pid"
}

remove_pid_file() {
    local service_name=$1
    rm -f "$RUN_DIR/pid/$service_name.pid"
}

is_running() {
    local service_name=$1
    local pid_file="$RUN_DIR/pid/$service_name.pid"
    
    if [ -f "$pid_file" ]; then
        local pid=$(cat "$pid_file")
        if kill -0 "$pid" 2>/dev/null; then
            return 0
        fi
    fi
    return 1
}

cleanup_stale_pids() {
    for pid_file in "$RUN_DIR/pid"/*.pid; do
        [ -f "$pid_file" ] || continue
        local pid=$(cat "$pid_file")
        if ! kill -0 "$pid" 2>/dev/null; then
            rm -f "$pid_file"
        fi
    done
}

init_runtime() {
    chmod 755 "$RUN_DIR"
    chmod 755 "$RUN_DIR/pid"
    chmod 755 "$RUN_DIR/lock"
    chmod 777 "$RUN_DIR/sockets"
}

case "$1" in
    init)
        init_runtime
        echo "Initialized /var/run"
        ;;
    create_pid)
        create_pid_file "$2" "$3"
        ;;
    remove_pid)
        remove_pid_file "$2"
        ;;
    is_running)
        is_running "$2"
        ;;
    cleanup)
        cleanup_stale_pids
        echo "Cleaned stale PID files"
        ;;
    *)
        echo "Usage: $0 {init|create_pid|remove_pid|is_running|cleanup}"
        exit 1
        ;;
esac

