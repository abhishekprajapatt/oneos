#!/bin/bash

PID_DIR="/var/run/pid"

pid_init() {
    mkdir -p "$PID_DIR"
    chmod 755 "$PID_DIR"
    echo "✓ PID directory initialized: $PID_DIR"
}

pid_create() {
    local service=$1
    local pid=$2
    
    if [ -z "$service" ] || [ -z "$pid" ]; then
        echo "Usage: $0 create <service> <pid>"
        return 1
    fi
    
    echo "$pid" > "$PID_DIR/$service.pid"
    echo "created=$(date +%s)" >> "$PID_DIR/$service.pid"
    echo "service=$service" >> "$PID_DIR/$service.pid"
    echo "✓ PID file created: $PID_DIR/$service.pid (PID: $pid)"
}

pid_read() {
    local service=$1
    local pidfile="$PID_DIR/$service.pid"
    
    if [ ! -f "$pidfile" ]; then
        echo "PID file not found: $pidfile"
        return 1
    fi
    
    head -n1 "$pidfile"
}

pid_is_running() {
    local service=$1
    local pidfile="$PID_DIR/$service.pid"
    
    if [ ! -f "$pidfile" ]; then
        echo "Service not tracked: $service"
        return 1
    fi
    
    local pid=$(head -n1 "$pidfile")
    if kill -0 "$pid" 2>/dev/null; then
        echo "✓ Service is running: $service (PID: $pid)"
        return 0
    else
        echo "✗ Service is not running: $service"
        rm -f "$pidfile"
        return 1
    fi
}

pid_remove() {
    local service=$1
    local pidfile="$PID_DIR/$service.pid"
    
    if [ ! -f "$pidfile" ]; then
        echo "PID file not found: $pidfile"
        return 1
    fi
    
    rm -f "$pidfile"
    echo "✓ PID file removed: $pidfile"
}

pid_list() {
    if [ ! -d "$PID_DIR" ]; then
        echo "PID directory not found: $PID_DIR"
        return 1
    fi
    
    echo "Service PID Files:"
    echo "======================="
    
    local count=0
    for pidfile in "$PID_DIR"/*.pid; do
        if [ -f "$pidfile" ]; then
            local service=$(basename "$pidfile" .pid)
            local pid=$(head -n1 "$pidfile")
            
            if kill -0 "$pid" 2>/dev/null; then
                echo "✓ $service: $pid (running)"
            else
                echo "✗ $service: $pid (stale)"
            fi
            
            ((count++))
        fi
    done
    
    if [ $count -eq 0 ]; then
        echo "No PID files found"
    else
        echo "======================="
        echo "Total: $count services"
    fi
}

pid_cleanup() {
    if [ ! -d "$PID_DIR" ]; then
        echo "PID directory not found: $PID_DIR"
        return 1
    fi
    
    local cleaned=0
    
    for pidfile in "$PID_DIR"/*.pid; do
        if [ -f "$pidfile" ]; then
            local pid=$(head -n1 "$pidfile")
            
            if ! kill -0 "$pid" 2>/dev/null; then
                local service=$(basename "$pidfile" .pid)
                rm -f "$pidfile"
                echo "✓ Removed stale PID: $service (was $pid)"
                ((cleaned++))
            fi
        fi
    done
    
    echo "Cleanup complete: $cleaned stale PID files removed"
}

pid_count() {
    if [ ! -d "$PID_DIR" ]; then
        echo "0"
        return 0
    fi
    
    ls -1 "$PID_DIR"/*.pid 2>/dev/null | wc -l
}

pid_monitor() {
    echo "Monitoring PID Directory: $PID_DIR"
    echo "======================================="
    
    while true; do
        clear
        echo "OneOS PID Monitor ($(date '+%Y-%m-%d %H:%M:%S'))"
        echo "======================================="
        echo ""
        
        local total=$(pid_count)
        local running=0
        local stale=0
        
        for pidfile in "$PID_DIR"/*.pid; do
            if [ -f "$pidfile" ]; then
                local pid=$(head -n1 "$pidfile")
                
                if kill -0 "$pid" 2>/dev/null; then
                    ((running++))
                else
                    ((stale++))
                fi
            fi
        done
        
        echo "Total PID Files: $total"
        echo "Running: $running"
        echo "Stale: $stale"
        echo ""
        
        pid_list
        
        echo ""
        echo "Press Ctrl+C to exit. Refreshing in 5 seconds..."
        sleep 5
    done
}

pid_size() {
    if [ ! -d "$PID_DIR" ]; then
        echo "0"
        return 0
    fi
    
    du -sh "$PID_DIR" | awk '{print $1}'
}

case "${1:-help}" in
    init)
        pid_init
        ;;
    create)
        pid_create "$2" "$3"
        ;;
    read)
        pid_read "$2"
        ;;
    is-running)
        pid_is_running "$2"
        ;;
    remove)
        pid_remove "$2"
        ;;
    list)
        pid_list
        ;;
    cleanup)
        pid_cleanup
        ;;
    count)
        pid_count
        ;;
    monitor)
        pid_monitor
        ;;
    size)
        pid_size
        ;;
    help|*)
        cat << 'EOF'
OneOS PID Management Script

Usage: oneos-pid-manager <command> [options]

Commands:
  init                    Initialize PID directory
  create <service> <pid>  Create PID file
  read <service>          Read PID file
  is-running <service>    Check if service is running
  remove <service>        Remove PID file
  list                    List all PID files
  cleanup                 Clean stale PID files
  count                   Get total PID count
  monitor                 Monitor PID directory (continuous)
  size                    Get PID directory size
  help                    Show this help message

Examples:
  oneos-pid-manager init
  oneos-pid-manager create sshd 1234
  oneos-pid-manager is-running sshd
  oneos-pid-manager list
  oneos-pid-manager cleanup
EOF
        ;;
esac

