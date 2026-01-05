#!/bin/bash

VAR_DIR="/var"

init_var_structure() {
    chmod 755 "$VAR_DIR"
    chmod 755 "$VAR_DIR/cache"
    chmod 755 "$VAR_DIR/backups"
    chmod 755 "$VAR_DIR/crash"
    chmod 755 "$VAR_DIR/lib"
    chmod 755 "$VAR_DIR/log"
    chmod 755 "$VAR_DIR/run"
    chmod 755 "$VAR_DIR/spool"
    chmod 777 "$VAR_DIR/tmp"
}

cleanup_var_tmp() {
    find "$VAR_DIR/tmp" -type f -atime +7 -delete 2>/dev/null
    find "$VAR_DIR/cache" -type f -atime +30 -delete 2>/dev/null
}

init_package_managers() {
    mkdir -p "$VAR_DIR/lib/apt/lists"
    mkdir -p "$VAR_DIR/lib/apt/partial"
    mkdir -p "$VAR_DIR/lib/dpkg/info"
    mkdir -p "$VAR_DIR/lib/rpm"
    mkdir -p "$VAR_DIR/lib/yum"
}

monitor_var_usage() {
    echo "=== /var Directory Usage ==="
    du -sh "$VAR_DIR"/*
    echo "=== Cache Usage ==="
    du -sh "$VAR_DIR/cache"/*
    echo "=== Log Usage ==="
    du -sh "$VAR_DIR/log"/*
}

case "$1" in
    init)
        init_var_structure
        init_package_managers
        echo "Initialized /var structure"
        ;;
    cleanup)
        cleanup_var_tmp
        echo "Cleaned temporary files"
        ;;
    monitor)
        monitor_var_usage
        ;;
    *)
        echo "Usage: $0 {init|cleanup|monitor}"
        exit 1
        ;;
esac

