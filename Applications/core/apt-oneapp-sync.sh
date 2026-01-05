#!/bin/bash

set -e

STORE_DB="/var/lib/oneos-store/store.db"
ONEAPP_CACHE="/var/cache/oneos-store/apps"
WINE_PREFIX="$HOME/.oneos/wine"
APT_LISTS="/var/lib/apt/lists"

init_env() {
    echo "[*] Initializing OneOS Package Manager bridge..."
    mkdir -p "$ONEAPP_CACHE"
    mkdir -p "$(dirname "$STORE_DB")"
    
    if [ ! -f "$STORE_DB" ]; then
        touch "$STORE_DB"
        echo "[✓] Store database created: $STORE_DB"
    fi
    
    if [ ! -d "$WINE_PREFIX" ]; then
        mkdir -p "$WINE_PREFIX"
        echo "[✓] Wine prefix created: $WINE_PREFIX"
    fi
}

apt_sync_packages() {
    echo "[*] Syncing APT packages with OneOS Store..."
    
    local installed_pkgs=$(dpkg -l | grep '^ii' | awk '{print $2}')
    
    local count=0
    while IFS= read -r pkg; do
        [ -z "$pkg" ] && continue
        
        if ! grep -q "name.*$pkg" "$STORE_DB" 2>/dev/null; then
            local version=$(dpkg -l | grep "^ii.*$pkg" | awk '{print $3}')
            
            sqlite3 "$STORE_DB" <<EOF 2>/dev/null || true
INSERT INTO apps (name, version, category, installed) 
VALUES ('$pkg', '$version', 'System', 1);
EOF
            count=$((count + 1))
        fi
    done <<< "$installed_pkgs"
    
    echo "[✓] Synced $count packages from APT"
}

detect_exe_apps() {
    echo "[*] Scanning for .exe applications (Windows binaries)..."
    
    local exe_count=0
    local scan_paths=("/opt" "/usr/local" "/home" "/Applications")
    
    for scan_path in "${scan_paths[@]}"; do
        [ ! -d "$scan_path" ] && continue
        
        while IFS= read -r exe_file; do
            if [ -f "$exe_file" ]; then
                local app_name=$(basename "$exe_file" .exe)
                local app_category="Windows Apps"
                
                if ! sqlite3 "$STORE_DB" "SELECT name FROM apps WHERE name='$app_name'" 2>/dev/null | grep -q "$app_name"; then
                    echo "  [*] Wrapping: $exe_file"
                    
                    create_wine_wrapper "$app_name" "$exe_file"
                    
                    sqlite3 "$STORE_DB" <<EOF 2>/dev/null || true
INSERT INTO apps (name, version, category, installed, icon_url) 
VALUES ('$app_name', '1.0', '$app_category', 1, 'https:
EOF
                    exe_count=$((exe_count + 1))
                fi
            fi
        done < <(find "$scan_path" -name "*.exe" 2>/dev/null)
    done
    
    echo "[✓] Wrapped $exe_count .exe applications"
}

create_wine_wrapper() {
    local app_name=$1
    local exe_path=$2
    local wrapper_dir="/usr/local/bin"
    local wrapper_file="$wrapper_dir/oneos-$app_name"
    
    cat > "$wrapper_file" << 'WINE_SCRIPT'
#!/bin/bash

APP_NAME="$0"
EXE_PATH="$1"

export WINEPREFIX="${WINEPREFIX:-$HOME/.oneos/wine}"
export WINEARCH="x86_64"
export DISPLAY=":0"

if [ ! -d "$WINEPREFIX" ]; then
    echo "[*] Initializing Wine prefix: $WINEPREFIX"
    wineboot -i
fi

if command -v wine64 &> /dev/null; then
    wine64 "$EXE_PATH" "${@:2}"
elif command -v wine &> /dev/null; then
    wine "$EXE_PATH" "${@:2}"
else
    echo "[!] Wine not installed - install via: apt install wine wine32 wine64"
    exit 1
fi
WINE_SCRIPT
    
    chmod +x "$wrapper_file"
    echo "[✓] Wrapper created: $wrapper_file"
}

install_apt_hooks() {
    echo "[*] Installing APT hooks for OneOS Store sync..."
    
    local hook_dir="/etc/apt/apt.conf.d"
    mkdir -p "$hook_dir"
    
    cat > "$hook_dir/99oneos-sync-post" << 'APT_HOOK'
APT::Post-Invoke {"/usr/local/bin/oneos-apt-sync check";};
DPkg::Post-Invoke {"/usr/local/bin/oneos-apt-sync check";};
APT_HOOK
    
    cat > "/usr/local/bin/oneos-apt-sync" << 'SYNC_SCRIPT'
#!/bin/bash
case "$1" in
    check)
        /usr/local/bin/store-sync-packages
        ;;
    *)
        echo "Usage: $0 {check}"
        ;;
esac
SYNC_SCRIPT
    
    chmod +x "/usr/local/bin/oneos-apt-sync"
    echo "[✓] APT hooks installed"
}

auto_wrap_popular_exe() {
    echo "[*] Auto-wrapping popular .exe applications..."
    
    local popular_apps=(
        "vlc.exe:VideoLAN VLC:Video:https:
        "7z.exe:7-Zip:Utility:https:
        "putty.exe:PuTTY SSH:Internet:https:
        "python.exe:Python Runtime:Development:https:
        "nodejs.exe:Node.js:Development:https:
    )
    
    for app_spec in "${popular_apps[@]}"; do
        IFS=':' read -r exe_name display_name category icon_url <<< "$app_spec"
        
        if command -v "$exe_name" &> /dev/null || find / -name "$exe_name" 2>/dev/null | head -1; then
            local exe_path=$(find / -name "$exe_name" -type f 2>/dev/null | head -1)
            
            if [ -n "$exe_path" ]; then
                echo "  [*] Found: $display_name at $exe_path"
                create_wine_wrapper "${display_name
                
                sqlite3 "$STORE_DB" <<EOF 2>/dev/null || true
INSERT OR IGNORE INTO apps (name, version, category, installed, icon_url, requires_wine) 
VALUES ('$display_name', '1.0', '$category', 1, '$icon_url', 1);
EOF
            fi
        fi
    done
    
    echo "[✓] Popular .exe apps wrapped"
}

show_status() {
    echo ""
    echo "╔═══════════════════════════════════════════╗"
    echo "║   OneOS Package Manager Integration       ║"
    echo "╚═══════════════════════════════════════════╝"
    echo ""
    
    if [ -f "$STORE_DB" ]; then
        local total_apps=$(sqlite3 "$STORE_DB" "SELECT COUNT(*) FROM apps;" 2>/dev/null || echo "0")
        local installed_apps=$(sqlite3 "$STORE_DB" "SELECT COUNT(*) FROM apps WHERE installed=1;" 2>/dev/null || echo "0")
        local wine_apps=$(sqlite3 "$STORE_DB" "SELECT COUNT(*) FROM apps WHERE requires_wine=1;" 2>/dev/null || echo "0")
        
        echo "📦 Total apps in store:        $total_apps"
        echo "✓  Installed apps:             $installed_apps"
        echo "🍷 Wine-wrapped apps:          $wine_apps"
        echo "📂 Store database:             $STORE_DB"
        echo "🗂️  Cache directory:            $ONEAPP_CACHE"
        echo "🍇 Wine prefix:                $WINE_PREFIX"
    fi
    
    echo ""
    echo "Usage:"
    echo "  apt --oneapp-sync
    echo "  apt install <package>
    echo ""
}

main() {
    case "${1:-sync}" in
        sync)
            init_env
            apt_sync_packages
            detect_exe_apps
            auto_wrap_popular_exe
            show_status
            ;;
        wrap)
            [ -n "$2" ] && [ -n "$3" ] && create_wine_wrapper "$2" "$3"
            ;;
        detect)
            detect_exe_apps
            ;;
        status)
            show_status
            ;;
        *)
            echo "OneOS Package Manager - apt --oneapp-sync bridge"
            echo "Usage: $0 {sync|wrap|detect|status}"
            exit 1
            ;;
    esac
}

main "$@"

