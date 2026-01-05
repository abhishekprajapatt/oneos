#!/bin/bash

set -e

CLIPBOARD_DIR="${XDG_DATA_HOME:-$HOME/.local/share}/gpaste"
CLIPBOARD_DB="$CLIPBOARD_DIR/clipboard.db"
CLIPBOARD_HISTORY_FILE="$CLIPBOARD_DIR/history"
MAX_HISTORY=100
SEARCH_THRESHOLD=3

init_clipboard_db() {
    mkdir -p "$CLIPBOARD_DIR"
    
    if [ ! -f "$CLIPBOARD_DB" ]; then
        sqlite3 "$CLIPBOARD_DB" << 'EOF'
CREATE TABLE IF NOT EXISTS clipboard_items (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    content TEXT NOT NULL,
    timestamp INTEGER NOT NULL,
    source TEXT,
    type TEXT,
    size INTEGER,
    hash TEXT UNIQUE,
    encrypted INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS clipboard_tags (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    item_id INTEGER NOT NULL,
    tag TEXT NOT NULL,
    FOREIGN KEY (item_id) REFERENCES clipboard_items(id)
);

CREATE TABLE IF NOT EXISTS clipboard_sync (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    item_id INTEGER NOT NULL,
    device TEXT NOT NULL,
    synced_at INTEGER NOT NULL,
    status TEXT,
    FOREIGN KEY (item_id) REFERENCES clipboard_items(id)
);

CREATE INDEX idx_timestamp ON clipboard_items(timestamp DESC);
CREATE INDEX idx_hash ON clipboard_items(hash);
CREATE INDEX idx_device ON clipboard_sync(device);
EOF
        echo "[✓] Clipboard database initialized"
    fi
}

add_clipboard_entry() {
    local content="$1"
    local source="${2:-system}"
    local type="${3:-text}"
    
    local content_hash=$(echo -n "$content" | sha256sum | cut -d' ' -f1)
    local size=${
    local timestamp=$(date +%s)
    
    sqlite3 "$CLIPBOARD_DB" << EOF
INSERT OR IGNORE INTO clipboard_items 
    (content, timestamp, source, type, size, hash)
VALUES 
    ('$content', $timestamp, '$source', '$type', $size, '$content_hash');
EOF
    
    cleanup_history
}

cleanup_history() {
    local count=$(sqlite3 "$CLIPBOARD_DB" "SELECT COUNT(*) FROM clipboard_items;")
    
    if [ "$count" -gt "$MAX_HISTORY" ]; then
        local to_delete=$((count - MAX_HISTORY))
        sqlite3 "$CLIPBOARD_DB" << EOF
DELETE FROM clipboard_items WHERE id IN (
    SELECT id FROM clipboard_items 
    ORDER BY timestamp ASC 
    LIMIT $to_delete
);
EOF
        echo "[*] Cleaned $to_delete old clipboard entries"
    fi
}

search_clipboard() {
    local query="$1"
    
    echo "╔══════════════════════════════════════════╗"
    echo "║  Clipboard Search Results               ║"
    echo "╚══════════════════════════════════════════╝"
    echo ""
    
    sqlite3 -header -column "$CLIPBOARD_DB" << EOF
SELECT 
    id,
    substr(content, 1, 50) || CASE WHEN length(content) > 50 THEN '...' ELSE '' END as Content,
    datetime(timestamp, 'unixepoch') as Time,
    source as Source,
    type as Type
FROM clipboard_items
WHERE content LIKE '%$query%'
ORDER BY timestamp DESC
LIMIT 20;
EOF
}

show_clipboard_history() {
    local limit=${1:-20}
    
    echo "╔══════════════════════════════════════════╗"
    echo "║  Clipboard History (Last $limit items)   ║"
    echo "╚══════════════════════════════════════════╝"
    echo ""
    
    sqlite3 -header -column "$CLIPBOARD_DB" << EOF
SELECT 
    id,
    datetime(timestamp, 'unixepoch') as Time,
    substr(content, 1, 60) || CASE WHEN length(content) > 60 THEN '...' ELSE '' END as Content,
    source as Source,
    size as Size
FROM clipboard_items
ORDER BY timestamp DESC
LIMIT $limit;
EOF
}

restore_clipboard_item() {
    local item_id=$1
    
    local content=$(sqlite3 "$CLIPBOARD_DB" "SELECT content FROM clipboard_items WHERE id = $item_id;")
    
    if [ -z "$content" ]; then
        echo "[!] Item $item_id not found"
        return 1
    fi
    
    echo -n "$content" | xclip -selection clipboard
    
    echo "[✓] Restored item $item_id to clipboard"
    echo "Content: ${content:0:100}..."
}

sync_clipboard_to_device() {
    local device="$1"
    
    echo "[*] Syncing clipboard to $device..."
    
    if command -v kdeconnect-cli &>/dev/null; then
        sqlite3 "$CLIPBOARD_DB" << EOF
SELECT content FROM clipboard_items 
WHERE id = (SELECT MAX(id) FROM clipboard_items)
EOF
        
        echo "[✓] Synced to $device"
    else
        echo "[!] KDE Connect not available"
    fi
}

export_clipboard_history() {
    local output_file="${1:-clipboard_export.txt}"
    
    echo "[*] Exporting clipboard history to $output_file..."
    
    sqlite3 "$CLIPBOARD_DB" << EOF > "$output_file"
SELECT 
    datetime(timestamp, 'unixepoch') || ' | ' || source || ' | ' || content
FROM clipboard_items
ORDER BY timestamp DESC;
EOF
    
    echo "[✓] Exported to $output_file"
}

clear_clipboard_history() {
    local confirm=$1
    
    if [ "$confirm" != "-y" ]; then
        read -p "Clear all clipboard history? (y/n) " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            echo "[*] Aborted"
            return 1
        fi
    fi
    
    sqlite3 "$CLIPBOARD_DB" "DELETE FROM clipboard_items;"
    sqlite3 "$CLIPBOARD_DB" "DELETE FROM clipboard_tags;"
    sqlite3 "$CLIPBOARD_DB" "DELETE FROM clipboard_sync;"
    
    echo "[✓] Clipboard history cleared"
}

show_statistics() {
    echo "╔══════════════════════════════════════════╗"
    echo "║  Clipboard Statistics                   ║"
    echo "╚══════════════════════════════════════════╝"
    echo ""
    
    sqlite3 -header -column "$CLIPBOARD_DB" << 'EOF'
SELECT 
    (SELECT COUNT(*) FROM clipboard_items) as 'Total Items',
    (SELECT SUM(size) FROM clipboard_items) as 'Total Size (bytes)',
    (SELECT COUNT(DISTINCT source) FROM clipboard_items) as 'Sources',
    (SELECT COUNT(DISTINCT type) FROM clipboard_items) as 'Types';

SELECT '' as '';

SELECT 'Items by Source:' as '';
SELECT source as Source, COUNT(*) as Count 
FROM clipboard_items 
GROUP BY source 
ORDER BY Count DESC;

SELECT '' as '';

SELECT 'Items by Type:' as '';
SELECT type as Type, COUNT(*) as Count 
FROM clipboard_items 
GROUP BY type 
ORDER BY Count DESC;
EOF
}

monitor_system_clipboard() {
    echo "[*] Monitoring system clipboard for changes..."
    echo "[*] Press Ctrl+C to stop"
    echo ""
    
    local last_hash=""
    
    while true; do
        local current_content=$(xclip -selection clipboard -o 2>/dev/null || echo "")
        local current_hash=$(echo -n "$current_content" | sha256sum | cut -d' ' -f1)
        
        if [ "$current_hash" != "$last_hash" ] && [ -n "$current_content" ]; then
            echo "[+] New clipboard entry detected"
            add_clipboard_entry "$current_content" "system" "text"
            last_hash="$current_hash"
        fi
        
        sleep 1
    done
}

tag_clipboard_item() {
    local item_id=$1
    local tag=$2
    
    sqlite3 "$CLIPBOARD_DB" << EOF
INSERT INTO clipboard_tags (item_id, tag)
VALUES ($item_id, '$tag');
EOF
    
    echo "[✓] Tagged item $item_id with '$tag'"
}

search_by_tag() {
    local tag=$1
    
    echo "╔══════════════════════════════════════════╗"
    echo "║  Results for tag: $tag                   ║"
    echo "╚══════════════════════════════════════════╝"
    echo ""
    
    sqlite3 -header -column "$CLIPBOARD_DB" << EOF
SELECT 
    i.id,
    datetime(i.timestamp, 'unixepoch') as Time,
    substr(i.content, 1, 50) || '...' as Content,
    group_concat(t.tag) as Tags
FROM clipboard_items i
LEFT JOIN clipboard_tags t ON i.id = t.item_id
WHERE t.tag LIKE '%$tag%'
GROUP BY i.id
ORDER BY i.timestamp DESC;
EOF
}

main() {
    init_clipboard_db
    
    case "${1:-help}" in
        init)
            init_clipboard_db
            ;;
        add)
            add_clipboard_entry "$2" "${3:-system}" "${4:-text}"
            ;;
        list|history)
            show_clipboard_history "${2:-20}"
            ;;
        search)
            search_clipboard "$2"
            ;;
        restore)
            restore_clipboard_item "$2"
            ;;
        sync)
            sync_clipboard_to_device "$2"
            ;;
        export)
            export_clipboard_history "${2:-clipboard_export.txt}"
            ;;
        clear)
            clear_clipboard_history "$2"
            ;;
        stats)
            show_statistics
            ;;
        monitor)
            monitor_system_clipboard
            ;;
        tag)
            tag_clipboard_item "$2" "$3"
            ;;
        tag-search)
            search_by_tag "$2"
            ;;
        *)
            cat << 'EOF'
GPaste Clipboard Manager

Usage: gpaste <command> [args]

Commands:
  init              Initialize clipboard database
  add <text>        Add text to clipboard history
  list [n]          Show last n items (default: 20)
  history [n]       Alias for list
  search <query>    Search clipboard history
  restore <id>      Restore item by ID to clipboard
  sync <device>     Sync clipboard to device
  export [file]     Export history to file
  clear [-y]        Clear all history
  stats             Show statistics
  monitor           Monitor system clipboard for changes
  tag <id> <tag>    Tag clipboard item
  tag-search <tag>  Search by tag

Examples:
  gpaste list 50
  gpaste search "meeting"
  gpaste restore 5
  gpaste stats
  gpaste monitor
EOF
            ;;
    esac
}

main "$@"

