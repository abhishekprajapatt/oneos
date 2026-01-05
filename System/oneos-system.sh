#!/bin/bash

set -e

SYSTEM_DIR="/System"
CORE_DIR="/System/core"
PREFS_DIR="/System/Preferences"
LIB_DIR="/System/Library"
LAUNCH_AGENTS_DIR="/System/Library/LaunchAgents"
LAUNCH_DAEMONS_DIR="/System/Library/LaunchDaemons"
LOG_FILE="/var/log/oneos-system.log"

log_message() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a $LOG_FILE
}

print_header() {
    echo ""
    echo "========================================"
    echo "$1"
    echo "========================================"
}

core_list() {
    print_header "System Core Components"
    
    if [[ ! -d "$CORE_DIR" ]]; then
        echo "No core components found"
        return 0
    fi
    
    printf "%-35s %-15s %-12s %-10s\n" "COMPONENT" "TYPE" "STATUS" "PRIORITY"
    printf "%-35s %-15s %-12s %-10s\n" "-----------" "------" "------" "--------"
    
    local count=0
    for comp_file in "$CORE_DIR"status=1/" "$comp_file"
    sed -i "s/^loaded=.*/loaded=1/" "$comp_file"
    log_message "Loaded core component: $name"
    echo "Component loaded: $name"
}

core_unload() {
    local name="$1"
    
    if [[ -z "$name" ]]; then
        echo "Usage: oneos-system core unload <name>"
        return 1
    fi
    
    local comp_file="$CORE_DIR/${name}.comp"
    [[ ! -f "$comp_file" ]] && { echo "Component not found: $name"; return 1; }
    
    sed -i "s/^status=.*/status=0/" "$comp_file"
    sed -i "s/^loaded=.*/loaded=0/" "$comp_file"
    log_message "Unloaded core component: $name"
    echo "Component unloaded: $name"
}

pref_set() {
    local domain="$1"
    local key="$2"
    local value="$3"
    local type="${4:-1}"
    
    if [[ -z "$domain" ]] || [[ -z "$key" ]] || [[ -z "$value" ]]; then
        echo "Usage: oneos-system pref set <domain> <key> <value> [type]"
        return 1
    fi
    
    local domain_dir="$PREFS_DIR/$domain"
    mkdir -p "$domain_dir"
    
    local pref_file="$domain_dir/${key}.pref"
    
    cat > "$pref_file" << EOF
domain=$domain
key=$key
value=$value
type=$type
readonly=0
modified=$(date +%s)
EOF
    
    chmod 644 "$pref_file"
    log_message "Set preference: $domain.$key = $value"
    echo "Preference set: $domain.$key = $value"
}

pref_get() {
    local domain="$1"
    local key="$2"
    
    if [[ -z "$domain" ]] || [[ -z "$key" ]]; then
        echo "Usage: oneos-system pref get <domain> <key>"
        return 1
    fi
    
    local pref_file="$PREFS_DIR/$domain/${key}.pref"
    [[ ! -f "$pref_file" ]] && { echo "Preference not found: $domain.$key"; return 1; }
    
    grep "^value=" "$pref_file" | cut -d= -f2-
}

pref_list_domain() {
    local domain="$1"
    
    if [[ -z "$domain" ]]; then
        echo "Usage: oneos-system pref list <domain>"
        return 1
    fi
    
    local domain_dir="$PREFS_DIR/$domain"
    [[ ! -d "$domain_dir" ]] && { echo "Domain not found: $domain"; return 1; }
    
    print_header "Preferences for domain: $domain"
    
    printf "%-40s %-15s %-30s\n" "KEY" "TYPE" "VALUE"
    printf "%-40s %-15s %-30s\n" "---" "----" "-----"
    
    local count=0
    for pref_file in "$domain_dir"loaded=1/" "$lib_file"
    sed -i "s/^status=.*/status=2/" "$lib_file"
    log_message "Loaded library: $name"
    echo "Library loaded: $name"
}

launch_agent_create() {
    local label="$1"
    local program="$2"
    
    if [[ -z "$label" ]] || [[ -z "$program" ]]; then
        echo "Usage: oneos-system launch agent-create <label> <program>"
        return 1
    fi
    
    local plist_file="$LAUNCH_AGENTS_DIR/${label}.plist"
    
    cat > "$plist_file" << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0">
<dict>
  <key>Label</key>
  <string>LABEL_PLACEHOLDER</string>
  <key>Program</key>
  <string>PROGRAM_PLACEHOLDER</string>
  <key>RunAtLoad</key>
  <false/>
  <key>KeepAlive</key>
  <false/>
  <key>StandardOutPath</key>
  <string>/var/log/LABEL_PLACEHOLDER.log</string>
  <key>StandardErrorPath</key>
  <string>/var/log/LABEL_PLACEHOLDER.err</string>
</dict>
</plist>
EOF
    
    sed -i "s|LABEL_PLACEHOLDER|$label|g" "$plist_file"
    sed -i "s|PROGRAM_PLACEHOLDER|$program|g" "$plist_file"
    
    chmod 644 "$plist_file"
    log_message "Created launch agent: $label"
    echo "Launch agent created: $label"
}

launch_daemon_create() {
    local label="$1"
    local program="$2"
    
    if [[ -z "$label" ]] || [[ -z "$program" ]]; then
        echo "Usage: oneos-system launch daemon-create <label> <program>"
        return 1
    fi
    
    local plist_file="$LAUNCH_DAEMONS_DIR/${label}.plist"
    
    cat > "$plist_file" << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0">
<dict>
  <key>Label</key>
  <string>LABEL_PLACEHOLDER</string>
  <key>Program</key>
  <string>PROGRAM_PLACEHOLDER</string>
  <key>RunAtLoad</key>
  <false/>
  <key>KeepAlive</key>
  <true/>
  <key>StandardOutPath</key>
  <string>/var/log/LABEL_PLACEHOLDER.log</string>
  <key>StandardErrorPath</key>
  <string>/var/log/LABEL_PLACEHOLDER.err</string>
</dict>
</plist>
EOF
    
    sed -i "s|LABEL_PLACEHOLDER|$label|g" "$plist_file"
    sed -i "s|PROGRAM_PLACEHOLDER|$program|g" "$plist_file"
    
    chmod 644 "$plist_file"
    log_message "Created launch daemon: $label"
    echo "Launch daemon created: $label"
}

launch_agent_list() {
    print_header "Launch Agents"
    
    if [[ ! -d "$LAUNCH_AGENTS_DIR" ]]; then
        echo "No launch agents found"
        return 0
    fi
    
    printf "%-50s %-15s\n" "LABEL" "STATUS"
    printf "%-50s %-15s\n" "-----" "------"
    
    local count=0
    for plist_file in "$LAUNCH_AGENTS_DIR"/*.plist 2>/dev/null; do
        [[ ! -f "$plist_file" ]] && continue
        
        local label=$(basename "$plist_file" .plist)
        printf "%-50s %-15s\n" "$label" "Loaded"
        ((count++))
    done
    
    echo "Total: $count agents"
}

launch_daemon_list() {
    print_header "Launch Daemons"
    
    if [[ ! -d "$LAUNCH_DAEMONS_DIR" ]]; then
        echo "No launch daemons found"
        return 0
    fi
    
    printf "%-50s %-15s\n" "LABEL" "STATUS"
    printf "%-50s %-15s\n" "-----" "------"
    
    local count=0
    for plist_file in "$LAUNCH_DAEMONS_DIR"/*.plist 2>/dev/null; do
        [[ ! -f "$plist_file" ]] && continue
        
        local label=$(basename "$plist_file" .plist)
        printf "%-50s %-15s\n" "$label" "Loaded"
        ((count++))
    done
    
    echo "Total: $count daemons"
}

launch_delete() {
    local label="$1"
    local type="${2:-agent}"
    
    if [[ -z "$label" ]]; then
        echo "Usage: oneos-system launch delete <label> [agent|daemon]"
        return 1
    fi
    
    local dir="$LAUNCH_AGENTS_DIR"
    [[ "$type" == "daemon" ]] && dir="$LAUNCH_DAEMONS_DIR"
    
    local plist_file="$dir/${label}.plist"
    [[ ! -f "$plist_file" ]] && { echo "Launch entry not found: $label"; return 1; }
    
    rm -f "$plist_file"
    log_message "Deleted launch $type: $label"
    echo "Launch $type deleted: $label"
}

show_help() {
    cat << 'EOF'
OneOS System Management

CORE Commands:
  oneos-system core list              List all core components
  oneos-system core register <name> [type]
                                      Register a core component
  oneos-system core load <name>       Load a core component
  oneos-system core unload <name>     Unload a core component

PREFERENCES Commands:
  oneos-system pref set <domain> <key> <value> [type]
                                      Set a preference
  oneos-system pref get <domain> <key>
                                      Get a preference value
  oneos-system pref list <domain>     List preferences for domain
  oneos-system pref delete <domain> <key>
                                      Delete a preference

LIBRARY Commands:
  oneos-system lib register <name> <path> [type]
                                      Register a library
  oneos-system lib list               List all libraries
  oneos-system lib load <name>        Load a library

LAUNCH Commands:
  oneos-system launch agent-create <label> <program>
                                      Create launch agent
  oneos-system launch daemon-create <label> <program>
                                      Create launch daemon
  oneos-system launch agent-list      List launch agents
  oneos-system launch daemon-list     List launch daemons
  oneos-system launch delete <label> [agent|daemon]
                                      Delete launch entry

COMPONENT TYPES:
  1 = Kernel       2 = Drivers      3 = Runtime      4 = Libraries
  5 = Utilities    6 = Services     7 = Security     8 = Networking

PREFERENCE TYPES:
  1 = String       2 = Integer      3 = Boolean      4 = Float
  5 = Array        6 = Binary

LIBRARY TYPES:
  1 = Framework    2 = Extension    3 = Font         4 = Plugin
  5 = Daemon       6 = Agent

Examples:
  oneos-system core register NetworkDriver 8
  oneos-system pref set com.oneos.display brightness 80 2
  oneos-system lib register Cocoa /System/Library/Frameworks/Cocoa.framework 1
  oneos-system launch daemon-create org.oneos.syslogd /usr/libexec/syslogd
EOF
}

case "${1:-help}" in
    core)
        case "${2:-help}" in
            list)       core_list ;;
            register)   core_register "$3" "$4" ;;
            load)       core_load "$3" ;;
            unload)     core_unload "$3" ;;
            *)          echo "Unknown core command: $2"; show_help ;;
        esac
        ;;
    pref|preference)
        case "${2:-help}" in
            set)        pref_set "$3" "$4" "$5" "$6" ;;
            get)        pref_get "$3" "$4" ;;
            list)       pref_list_domain "$3" ;;
            delete)     pref_delete "$3" "$4" ;;
            *)          echo "Unknown preference command: $2"; show_help ;;
        esac
        ;;
    lib|library)
        case "${2:-help}" in
            register)   lib_register "$3" "$4" "$5" ;;
            list)       lib_list ;;
            load)       lib_load "$3" ;;
            *)          echo "Unknown library command: $2"; show_help ;;
        esac
        ;;
    launch)
        case "${2:-help}" in
            agent-create)   launch_agent_create "$3" "$4" ;;
            daemon-create)  launch_daemon_create "$3" "$4" ;;
            agent-list)     launch_agent_list ;;
            daemon-list)    launch_daemon_list ;;
            delete)         launch_delete "$3" "$4" ;;
            *)              echo "Unknown launch command: $2"; show_help ;;
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

