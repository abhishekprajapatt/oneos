#!/bin/bash

ALERT_THRESHOLD_LOG=500000000
ALERT_THRESHOLD_CACHE=100000000
ALERT_THRESHOLD_TMP=50000000

check_log_size() {
    local size=$(du -sb /var/log | cut -f1)
    
    if [ $size -gt $ALERT_THRESHOLD_LOG ]; then
        echo "ALERT: Log size ($size bytes) exceeds threshold"
        /bin/var_manager log rotate
    fi
}

check_cache_size() {
    local size=$(du -sb /var/cache | cut -f1)
    
    if [ $size -gt $ALERT_THRESHOLD_CACHE ]; then
        echo "ALERT: Cache size ($size bytes) exceeds threshold"
        /bin/var_manager cache clear
    fi
}

check_tmp_size() {
    local size=$(du -sb /var/tmp | cut -f1)
    
    if [ $size -gt $ALERT_THRESHOLD_TMP ]; then
        echo "ALERT: Temp size ($size bytes) exceeds threshold"
        find /var/tmp -type f -delete 2>/dev/null
    fi
}

check_disk_space() {
    local available=$(df /var | awk 'NR==2 {print $4}')
    
    if [ $available -lt 1000000 ]; then
        echo "ALERT: Low disk space on /var partition"
        /bin/var_manager crash cleanup 0
        /bin/var_manager log rotate
    fi
}

check_stale_pids() {
    /bin/var_manager pid cleanup
}

verify_structure() {
    /bin/var_manager check
}

main() {
    echo "[$(date)] Starting /var monitoring"
    
    while true; do
        check_log_size
        check_cache_size
        check_tmp_size
        check_disk_space
        check_stale_pids
        
        sleep 60
    done
}

main "$@"

