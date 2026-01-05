#!/bin/bash

CLEANUP_INTERVAL=3600

cleanup_logs() {
    /bin/var_manager log rotate
}

cleanup_cache() {
    /bin/var_manager cache cleanup
}

cleanup_temp() {
    find /var/tmp -type f -atime +7 -delete 2>/dev/null
}

cleanup_backups() {
    find /var/backups -type f -mtime +30 -delete 2>/dev/null
}

cleanup_crashes() {
    /bin/var_manager crash cleanup 7
}

cleanup_spool() {
    find /var/spool -type f -mtime +14 -delete 2>/dev/null
}

main() {
    echo "[$(date)] Starting OneOS /var cleanup"
    
    cleanup_logs
    cleanup_cache
    cleanup_temp
    cleanup_backups
    cleanup_crashes
    cleanup_spool
    
    echo "[$(date)] Cleanup complete"
}

main "$@"

