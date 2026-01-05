#!/bin/bash

REGISTRY_DIR="/var/lib/registry"
CONFIG_DIR="/Windows/System32/config"
DRIVER_DIR="/Windows/System32/drivers"
PROGRAM_DATA_DIR="/Windows/ProgramData"

windows_init() {
    mkdir -p "$REGISTRY_DIR"
    mkdir -p "$CONFIG_DIR"
    mkdir -p "$DRIVER_DIR"
    mkdir -p "$PROGRAM_DATA_DIR"
    
    echo "✓ Windows system initialized"
}

registry_create_key() {
    local key=$1
    if [ -z "$key" ]; then
        echo "Usage: $0 registry-create <key>"
        return 1
    fi
    
    mkdir -p "$REGISTRY_DIR/$key"
    echo "✓ Registry key created: $key"
}

registry_set_value() {
    local key=$1
    local name=$2
    local value=$3
    
    if [ -z "$key" ] || [ -z "$name" ] || [ -z "$value" ]; then
        echo "Usage: $0 registry-set <key> <name> <value>"
        return 1
    fi
    
    echo "$value" > "$REGISTRY_DIR/$key/$name.val"
    echo "✓ Registry value set: $key/$name"
}

registry_get_value() {
    local key=$1
    local name=$2
    
    if [ -z "$key" ] || [ -z "$name" ]; then
        echo "Usage: $0 registry-get <key> <name>"
        return 1
    fi
    
    if [ -f "$REGISTRY_DIR/$key/$name.val" ]; then
        cat "$REGISTRY_DIR/$key/$name.val"
    else
        echo "Value not found"
        return 1
    fi
}

config_set() {
    local key=$1
    local value=$2
    
    if [ -z "$key" ] || [ -z "$value" ]; then
        echo "Usage: $0 config-set <key> <value>"
        return 1
    fi
    
    echo "$key=$value" >> "$CONFIG_DIR/system.conf"
    echo "✓ Configuration set: $key=$value"
}

config_get() {
    local key=$1
    
    if [ -z "$key" ]; then
        echo "Usage: $0 config-get <key>"
        return 1
    fi
    
    grep "^$key=" "$CONFIG_DIR/system.conf" | cut -d'=' -f2
}

config_list() {
    echo "System Configuration:"
    echo "======================================"
    grep -v "^
    echo "======================================"
}

driver_install() {
    local driver_name=$1
    local driver_path=$2
    
    if [ -z "$driver_name" ] || [ -z "$driver_path" ]; then
        echo "Usage: $0 driver-install <name> <path>"
        return 1
    fi
    
    if [ ! -f "$driver_path" ]; then
        echo "Driver file not found: $driver_path"
        return 1
    fi
    
    cp "$driver_path" "$DRIVER_DIR/$driver_name.sys"
    
    cat > "$DRIVER_DIR/$driver_name.inf" << EOF
name=$driver_name
installed=$(date +%s)
state=1
loaded=0
EOF
    
    echo "✓ Driver installed: $driver_name"
}

driver_list() {
    echo "Installed Drivers:"
    echo "======================================"
    
    for inf_file in "$DRIVER_DIR"; do
        if [ -d "$app_dir" ]; then
            app_name=$(basename "$app_dir")
            if [ -f "$app_dir/.app.info" ]; then
                version=$(grep "version=" "$app_dir/.app.info" | cut -d'=' -f2)
                echo "  - $app_name (v$version)"
            fi
        fi
    done
    
    echo "======================================"
    echo "Total: $(ls -1d "$PROGRAM_DATA_DIR" 2>/dev/null | wc -l)"
    echo ""
    
    echo "System Configuration:"
    echo "  File: $CONFIG_DIR/system.conf"
    echo "  Entries: $(grep -c "=" "$CONFIG_DIR/system.conf" 2>/dev/null || echo 0)"
    echo ""
    
    echo "Drivers:"
    echo "  Total: $(ls -1 "$DRIVER_DIR" 2>/dev/null | wc -l)"
    echo ""
}

show_help() {
    cat << 'EOF'
OneOS Windows System Management Script

Usage: oneos-windows <command> [options]

Registry Commands:
  registry-create <key>              Create registry key
  registry-set <key> <name> <value>  Set registry value
  registry-get <key> <name>          Get registry value

Config Commands:
  config-set <key> <value>           Set system configuration
  config-get <key>                   Get system configuration
  config-list                        List all configurations

Driver Commands:
  driver-install <name> <path>       Install driver
  driver-list                        List all drivers
  driver-enable <name>               Enable driver
  driver-disable <name>              Disable driver

Application Commands:
  app-install <name> <path> [ver]    Install application
  app-uninstall <name>               Uninstall application
  app-list                           List all applications

System Commands:
  init                               Initialize Windows system
  status                             Show system status
  help                               Show this help message

Examples:
  oneos-windows init
  oneos-windows driver-install nvidia /path/to/driver.sys
  oneos-windows app-install "MyApp" "/usr/bin/myapp"
  oneos-windows config-list
  oneos-windows status
EOF
}

case "${1:-help}" in
    init)
        windows_init
        ;;
    registry-create)
        registry_create_key "$2"
        ;;
    registry-set)
        registry_set_value "$2" "$3" "$4"
        ;;
    registry-get)
        registry_get_value "$2" "$3"
        ;;
    config-set)
        config_set "$2" "$3"
        ;;
    config-get)
        config_get "$2"
        ;;
    config-list)
        config_list
        ;;
    driver-install)
        driver_install "$2" "$3"
        ;;
    driver-list)
        driver_list
        ;;
    driver-enable)
        driver_enable "$2"
        ;;
    driver-disable)
        driver_disable "$2"
        ;;
    app-install)
        app_install "$2" "$3" "$4"
        ;;
    app-uninstall)
        app_uninstall "$2"
        ;;
    app-list)
        app_list
        ;;
    status)
        status
        ;;
    help|*)
        show_help
        ;;
esac

