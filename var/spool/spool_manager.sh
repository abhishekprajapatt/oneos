#!/bin/bash

SPOOL_DIR="/var/spool"

list_print_jobs() {
    echo "=== Print Queue ==="
    ls -lh "$SPOOL_DIR/print"/* 2>/dev/null || echo "No print jobs"
}

list_mail_queue() {
    echo "=== Mail Queue ==="
    ls -lh "$SPOOL_DIR/mail"/* 2>/dev/null || echo "No mail in queue"
}

list_cron_jobs() {
    echo "=== Scheduled Cron Jobs ==="
    ls -lh "$SPOOL_DIR/cron"/* 2>/dev/null || echo "No scheduled jobs"
}

clear_print_queue() {
    rm -f "$SPOOL_DIR/print"/*
    echo "Cleared print queue"
}

spool_usage() {
    echo "=== Spool Usage ==="
    du -sh "$SPOOL_DIR"/*
}

init_spool() {
    mkdir -p "$SPOOL_DIR/cron"
    mkdir -p "$SPOOL_DIR/mail"
    mkdir -p "$SPOOL_DIR/print"
    mkdir -p "$SPOOL_DIR/at"
    chmod 755 "$SPOOL_DIR"/*
}

case "$1" in
    print)
        list_print_jobs
        ;;
    mail)
        list_mail_queue
        ;;
    cron)
        list_cron_jobs
        ;;
    clear-print)
        clear_print_queue
        ;;
    usage)
        spool_usage
        ;;
    init)
        init_spool
        ;;
    *)
        echo "Usage: $0 {print|mail|cron|clear-print|usage|init}"
        ;;
esac

