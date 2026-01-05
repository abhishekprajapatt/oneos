#!/bin/bash

set -e

VAR_DIR="/var"
VERBOSE=0
DRY_RUN=0

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

create_directories() {
    log_info "Creating /var directory structure..."
    
    local dirs=(
        "$VAR_DIR"
        "$VAR_DIR/cache"
        "$VAR_DIR/cache/apt"
        "$VAR_DIR/cache/apt/archives"
        "$VAR_DIR/cache/apt/archives/partial"
        "$VAR_DIR/cache/fontconfig"
        "$VAR_DIR/cache/man"
        "$VAR_DIR/cache/package-manager"
        "$VAR_DIR/log"
        "$VAR_DIR/log/applications"
        "$VAR_DIR/log/services"
        "$VAR_DIR/log/audit"
        "$VAR_DIR/run"
        "$VAR_DIR/run/pid"
        "$VAR_DIR/run/lock"
        "$VAR_DIR/run/sockets"
        "$VAR_DIR/lib"
        "$VAR_DIR/lib/apt"
        "$VAR_DIR/lib/apt/lists"
        "$VAR_DIR/lib/apt/lists/partial"
        "$VAR_DIR/lib/apt/cache"
        "$VAR_DIR/lib/dpkg"
        "$VAR_DIR/lib/dpkg/info"
        "$VAR_DIR/lib/dpkg/updates"
        "$VAR_DIR/lib/dpkg/alternatives"
        "$VAR_DIR/lib/rpm"
        "$VAR_DIR/lib/rpm/signingkeys"
        "$VAR_DIR/lib/systemd"
        "$VAR_DIR/spool"
        "$VAR_DIR/spool/cron"
        "$VAR_DIR/spool/mail"
        "$VAR_DIR/spool/print"
        "$VAR_DIR/spool/at"
        "$VAR_DIR/tmp"
        "$VAR_DIR/backups"
        "$VAR_DIR/crash"
    )
    
    for dir in "${dirs[@]}"; do
        if [ ! -d "$dir" ]; then
            mkdir -p "$dir"
            log_info "Created: $dir"
        fi
    done
}

set_permissions() {
    log_info "Setting permissions..."
    
    chmod 755 "$VAR_DIR"
    chmod 755 "$VAR_DIR/cache"
    chmod 755 "$VAR_DIR/log"
    chmod 755 "$VAR_DIR/run"
    chmod 755 "$VAR_DIR/lib"
    chmod 755 "$VAR_DIR/spool"
    chmod 777 "$VAR_DIR/tmp"
    chmod 755 "$VAR_DIR/backups"
    chmod 755 "$VAR_DIR/crash"
    
    chmod 755 "$VAR_DIR/cache/apt"
    chmod 755 "$VAR_DIR/lib/apt/lists"
    chmod 755 "$VAR_DIR/lib/apt/lists/partial"
    
    chmod 755 "$VAR_DIR/lib/dpkg"
    chmod 755 "$VAR_DIR/lib/dpkg/info"
    chmod 755 "$VAR_DIR/lib/dpkg/updates"
    
    log_info "Permissions set successfully"
}

init_logs() {
    log_info "Initializing log files..."
    
    local logs=(
        "$VAR_DIR/log/syslog"
        "$VAR_DIR/log/auth.log"
        "$VAR_DIR/log/kernel.log"
        "$VAR_DIR/log/messages"
        "$VAR_DIR/log/secure"
        "$VAR_DIR/log/dmesg"
    )
    
    for logfile in "${logs[@]}"; do
        if [ ! -f "$logfile" ]; then
            touch "$logfile"
            chmod 640 "$logfile"
            log_info "Created: $logfile"
        fi
    done
}

init_dpkg_status() {
    log_info "Initializing DPKG status..."
    
    local status_file="$VAR_DIR/lib/dpkg/status"
    
    if [ ! -f "$status_file" ]; then
        touch "$status_file"
        chmod 644 "$status_file"
    fi
    
    local status_old="$VAR_DIR/lib/dpkg/status.old"
    if [ ! -f "$status_old" ]; then
        cp "$status_file" "$status_old"
    fi
    
    log_info "DPKG status initialized"
}

init_apt_state() {
    log_info "Initializing APT state..."
    
    local sources_list="$VAR_DIR/lib/apt/lists/sources.list"
    if [ ! -f "$sources_list" ]; then
        touch "$sources_list"
    fi
    
    log_info "APT state initialized"
}

check_structure() {
    log_info "Checking /var structure..."
    
    local required_dirs=(
        "$VAR_DIR"
        "$VAR_DIR/cache"
        "$VAR_DIR/log"
        "$VAR_DIR/run"
        "$VAR_DIR/lib"
        "$VAR_DIR/spool"
        "$VAR_DIR/tmp"
    )
    
    local missing=0
    
    for dir in "${required_dirs[@]}"; do
        if [ ! -d "$dir" ]; then
            log_error "Missing: $dir"
            ((missing++))
        fi
    done
    
    if [ $missing -eq 0 ]; then
        log_info "All required directories present ✓"
        return 0
    else
        log_error "Found $missing missing directories"
        return 1
    fi
}

print_status() {
    echo ""
    echo "=== OneOS /var System Status ==="
    echo ""
    echo "Directory Structure:"
    tree -L 2 "$VAR_DIR" 2>/dev/null || find "$VAR_DIR" -maxdepth 2 -type d | sort
    echo ""
    echo "Usage:"
    du -sh "$VAR_DIR" 2>/dev/null
    du -sh "$VAR_DIR"/* 2>/dev/null
    echo ""
}

initialize() {
    log_info "Starting OneOS /var system initialization..."
    echo ""
    
    create_directories
    echo ""
    
    set_permissions
    echo ""
    
    init_logs
    echo ""
    
    init_dpkg_status
    echo ""
    
    init_apt_state
    echo ""
    
    check_structure
    echo ""
    
    log_info "Initialization complete!"
}

show_help() {
    cat << EOF
OneOS /var System Management

Usage: $0 [COMMAND] [OPTIONS]

Commands:
    init              Initialize /var directory structure
    check             Check structure integrity
    status            Print system status
    cleanup           Clean temporary files
    help              Show this help message

Examples:
    $0 init
    $0 check
    $0 status
    
Environment Variables:
    VERBOSE           Enable verbose output
    DRY_RUN           Show what would be done without making changes

EOF
}

main() {
    case "${1:-init}" in
        init)
            initialize
            ;;
        check)
            check_structure
            ;;
        status)
            check_structure
            print_status
            ;;
        cleanup)
            log_info "Cleaning temporary files..."
            find "$VAR_DIR/tmp" -type f -atime +7 -delete 2>/dev/null
            find "$VAR_DIR/cache" -type f -atime +30 -delete 2>/dev/null
            log_info "Cleanup complete"
            ;;
        help)
            show_help
            ;;
        *)
            log_error "Unknown command: $1"
            show_help
            exit 1
            ;;
    esac
}

main "$@"

