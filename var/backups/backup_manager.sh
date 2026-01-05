#!/bin/bash

BACKUP_DIR="/var/backups"

create_backup() {
    local backup_name=$1
    local source_dir=$2
    local timestamp=$(date +%Y%m%d_%H%M%S)
    
    if [ -z "$backup_name" ] || [ -z "$source_dir" ]; then
        echo "Usage: create_backup <name> <source_directory>"
        return 1
    fi
    
    local backup_file="$BACKUP_DIR/${backup_name}_${timestamp}.tar.gz"
    
    tar -czf "$backup_file" "$source_dir" 2>/dev/null
    
    if [ $? -eq 0 ]; then
        echo "Backup created: $backup_file"
        return 0
    else
        echo "Backup failed"
        return 1
    fi
}

list_backups() {
    echo "=== Available Backups ==="
    ls -lh "$BACKUP_DIR"/*.tar.gz 2>/dev/null || echo "No backups found"
}

restore_backup() {
    local backup_file=$1
    local restore_dir=$2
    
    if [ ! -f "$backup_file" ]; then
        echo "Backup file not found: $backup_file"
        return 1
    fi
    
    mkdir -p "$restore_dir"
    tar -xzf "$backup_file" -C "$restore_dir"
    
    echo "Restored from: $backup_file"
}

cleanup_old_backups() {
    local days=${1:-30}
    find "$BACKUP_DIR" -name "*.tar.gz" -mtime +$days -delete
    echo "Removed backups older than $days days"
}

backup_size() {
    du -sh "$BACKUP_DIR"
}

case "$1" in
    create)
        create_backup "$2" "$3"
        ;;
    list)
        list_backups
        ;;
    restore)
        restore_backup "$2" "$3"
        ;;
    cleanup)
        cleanup_old_backups "$2"
        ;;
    size)
        backup_size
        ;;
    *)
        echo "Usage: $0 {create <name> <dir>|list|restore <file> <dir>|cleanup [days]|size}"
        ;;
esac

