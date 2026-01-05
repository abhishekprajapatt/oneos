#!/bin/bash

set -e

TASKS_DIR="/Tasks"
SCHEDULED_TASKS_DIR="/Tasks/scheduled-tasks"
SYSTEM_TASKS_DIR="/Tasks/system-tasks"
LOG_FILE="/var/log/oneos-tasks.log"

log_message() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a $LOG_FILE
}

print_header() {
    echo ""
    echo "========================================"
    echo "$1"
    echo "========================================"
}

scheduled_task_create() {
    local name="$1"
    local command="$2"
    local trigger="${3:-0}"
    
    if [[ -z "$name" ]] || [[ -z "$command" ]]; then
        echo "Usage: oneos-tasks scheduled create <name> <command> [trigger_type]"
        return 1
    fi
    
    local task_file="$SCHEDULED_TASKS_DIR/${name}.task"
    
    cat > "$task_file" << EOF
name=$name
command=$command
description=User-created scheduled task
trigger=$trigger
state=1
priority=5
highest_privileges=0
created=$(date +%s)
modified=$(date +%s)
last_run=0
next_run=0
run_count=0
failure_count=0
EOF
    
    chmod 644 "$task_file"
    log_message "Created scheduled task: $name"
    echo "Scheduled task created: $name"
}

scheduled_task_delete() {
    local name="$1"
    
    if [[ -z "$name" ]]; then
        echo "Usage: oneos-tasks scheduled delete <name>"
        return 1
    fi
    
    rm -f "$SCHEDULED_TASKS_DIR/${name}.task"
    rm -f "$SCHEDULED_TASKS_DIR/${name}.history"
    log_message "Deleted scheduled task: $name"
    echo "Scheduled task deleted: $name"
}

scheduled_task_list() {
    print_header "Scheduled Tasks List"
    
    if [[ ! -d "$SCHEDULED_TASKS_DIR" ]]; then
        echo "No scheduled tasks found"
        return 0
    fi
    
    printf "%-35s %-15s %-15s %-10s\n" "NAME" "TRIGGER" "STATE" "RUN_COUNT"
    printf "%-35s %-15s %-15s %-10s\n" "----" "-------" "-----" "---------"
    
    local count=0
    for task_file in "$SCHEDULED_TASKS_DIR"run_count=$run_count/" "$task_file"
    
    if [[ $exit_code -ne 0 ]]; then
        local failure_count=$(grep "^failure_count=" "$task_file" | cut -d= -f2)
        ((failure_count++))
        sed -i "s/^failure_count=.*/failure_count=$failure_count/" "$task_file"
    fi
    
    sed -i "s/^last_run=.*/last_run=$end_time/" "$task_file"
    
    log_message "Executed scheduled task: $name (exit_code: $exit_code)"
    echo "Task execution completed. Exit code: $exit_code"
}

scheduled_task_history() {
    local name="$1"
    
    if [[ -z "$name" ]]; then
        echo "Usage: oneos-tasks scheduled history <name>"
        return 1
    fi
    
    local history_file="$SCHEDULED_TASKS_DIR/${name}.history"
    
    print_header "Task History: $name"
    
    if [[ ! -f "$history_file" ]]; then
        echo "No execution history available"
        return 0
    fi
    
    cat "$history_file"
}

scheduled_task_enable() {
    local name="$1"
    
    if [[ -z "$name" ]]; then
        echo "Usage: oneos-tasks scheduled enable <name>"
        return 1
    fi
    
    local task_file="$SCHEDULED_TASKS_DIR/${name}.task"
    [[ ! -f "$task_file" ]] && { echo "Task not found: $name"; return 1; }
    
    sed -i "s/^state=.*/state=1/" "$task_file"
    log_message "Enabled scheduled task: $name"
    echo "Task enabled: $name"
}

scheduled_task_disable() {
    local name="$1"
    
    if [[ -z "$name" ]]; then
        echo "Usage: oneos-tasks scheduled disable <name>"
        return 1
    fi
    
    local task_file="$SCHEDULED_TASKS_DIR/${name}.task"
    [[ ! -f "$task_file" ]] && { echo "Task not found: $name"; return 1; }
    
    sed -i "s/^state=.*/state=0/" "$task_file"
    log_message "Disabled scheduled task: $name"
    echo "Task disabled: $name"
}

system_task_create() {
    local name="$1"
    local command="$2"
    local type="${3:-5}"
    
    if [[ -z "$name" ]] || [[ -z "$command" ]]; then
        echo "Usage: oneos-tasks system create <name> <command> [type]"
        return 1
    fi
    
    local task_file="$SYSTEM_TASKS_DIR/${name}.systask"
    
    cat > "$task_file" << EOF
name=$name
command=$command
description=User-created system task
type=$type
status=0
priority=5
critical=0
created=$(date +%s)
started=0
completed=0
execution_time_ms=0
retry_count=0
max_retries=3
EOF
    
    chmod 644 "$task_file"
    log_message "Created system task: $name"
    echo "System task created: $name"
}

system_task_delete() {
    local name="$1"
    
    if [[ -z "$name" ]]; then
        echo "Usage: oneos-tasks system delete <name>"
        return 1
    fi
    
    rm -f "$SYSTEM_TASKS_DIR/${name}.systask"
    rm -f "$SYSTEM_TASKS_DIR/${name}.log"
    log_message "Deleted system task: $name"
    echo "System task deleted: $name"
}

system_task_list() {
    print_header "System Tasks List"
    
    if [[ ! -d "$SYSTEM_TASKS_DIR" ]]; then
        echo "No system tasks found"
        return 0
    fi
    
    printf "%-35s %-15s %-15s %-10s\n" "NAME" "TYPE" "STATUS" "CRITICAL"
    printf "%-35s %-15s %-15s %-10s\n" "----" "----" "------" "--------"
    
    local count=0
    for task_file in "$SYSTEM_TASKS_DIR"status=$new_status/" "$task_file"
    sed -i "s/^started=.*/started=$start_time/" "$task_file"
    sed -i "s/^completed=.*/completed=$end_time/" "$task_file"
    
    log_message "Executed system task: $name (exit_code: $exit_code)"
    echo "System task execution completed. Exit code: $exit_code"
}

system_task_log() {
    local name="$1"
    
    if [[ -z "$name" ]]; then
        echo "Usage: oneos-tasks system log <name>"
        return 1
    fi
    
    local log_file="$SYSTEM_TASKS_DIR/${name}.log"
    
    print_header "System Task Log: $name"
    
    if [[ ! -f "$log_file" ]]; then
        echo "No log available"
        return 0
    fi
    
    cat "$log_file"
}

system_task_critical() {
    local name="$1"
    local value="$2"
    
    if [[ -z "$name" ]] || [[ -z "$value" ]]; then
        echo "Usage: oneos-tasks system critical <name> <0|1>"
        return 1
    fi
    
    local task_file="$SYSTEM_TASKS_DIR/${name}.systask"
    [[ ! -f "$task_file" ]] && { echo "Task not found: $name"; return 1; }
    
    sed -i "s/^critical=.*/critical=$value/" "$task_file"
    log_message "Set critical flag for system task: $name = $value"
    echo "Critical flag updated: $name = $value"
}

show_help() {
    cat << 'EOF'
OneOS Tasks Management System

SCHEDULED TASKS Commands:
  oneos-tasks scheduled create <name> <command> [trigger]
                                    Create a scheduled task
  oneos-tasks scheduled delete <name>
                                    Delete a scheduled task
  oneos-tasks scheduled list        List all scheduled tasks
  oneos-tasks scheduled run <name>  Execute a scheduled task
  oneos-tasks scheduled enable <name>
                                    Enable a scheduled task
  oneos-tasks scheduled disable <name>
                                    Disable a scheduled task
  oneos-tasks scheduled history <name>
                                    View task execution history

SYSTEM TASKS Commands:
  oneos-tasks system create <name> <command> [type]
                                    Create a system task
  oneos-tasks system delete <name>  Delete a system task
  oneos-tasks system list           List all system tasks
  oneos-tasks system run <name>     Execute a system task
  oneos-tasks system log <name>     View system task log
  oneos-tasks system critical <name> <0|1>
                                    Set critical flag

TRIGGER TYPES (scheduled):
  0 = Once  1 = Daily      2 = Weekly    3 = Monthly
  4 = Hourly  5 = On-Boot  6 = On-Idle  7 = On-Event

TASK TYPES (system):
  1 = Startup      2 = Shutdown     3 = Maintenance  4 = Backup
  5 = Cleanup      6 = Update       7 = Monitor      8 = Repair

Examples:
  oneos-tasks scheduled create backup-db "mysqldump > backup.sql" 2
  oneos-tasks system create cleanup-logs "rm /var/log/*.old" 5
  oneos-tasks scheduled run backup-db
  oneos-tasks system run cleanup-logs
  oneos-tasks scheduled history backup-db
  oneos-tasks system log cleanup-logs
EOF
}

case "${1:-help}" in
    scheduled)
        case "${2:-help}" in
            create)  scheduled_task_create "$3" "$4" "$5" ;;
            delete)  scheduled_task_delete "$3" ;;
            list)    scheduled_task_list ;;
            run)     scheduled_task_run "$3" ;;
            enable)  scheduled_task_enable "$3" ;;
            disable) scheduled_task_disable "$3" ;;
            history) scheduled_task_history "$3" ;;
            *)       echo "Unknown scheduled command: $2"; show_help ;;
        esac
        ;;
    system)
        case "${2:-help}" in
            create)   system_task_create "$3" "$4" "$5" ;;
            delete)   system_task_delete "$3" ;;
            list)     system_task_list ;;
            run)      system_task_run "$3" ;;
            log)      system_task_log "$3" ;;
            critical) system_task_critical "$3" "$4" ;;
            *)        echo "Unknown system command: $2"; show_help ;;
        esac
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        echo "Unknown command: $1"
        show_help
        exit 1
        ;;
esac

