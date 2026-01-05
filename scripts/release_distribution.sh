#!/bin/bash

set -e

VERSION="1.0"
RELEASE_DATE=$(date +%Y-%m-%d)
TIMESTAMP=$(date +%s)
ISO_PATH="dist/oneos-1.0-final.iso"
TORRENT_DIR="releases/torrents"
MIRROR_DIR="releases/mirrors"
CHECKSUMS_FILE="releases/CHECKSUMS.SHA256"

mkdir -p "$TORRENT_DIR" "$MIRROR_DIR"

GREEN='\033[0;32m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

log() { echo -e "${BLUE}[DIST]${NC} $1"; }
success() { echo -e "${GREEN}✓${NC} $1"; }

validate_iso() {
    log "Validating OneOS v1.0 ISO..."
    
    if [ ! -f "$ISO_PATH" ]; then
        echo "Error: ISO not found at $ISO_PATH"
        exit 1
    fi
    
    ISO_SIZE=$(stat -f%z "$ISO_PATH" 2>/dev/null || stat -c%s "$ISO_PATH" 2>/dev/null)
    ISO_SIZE_GB=$(echo "scale=2; $ISO_SIZE / 1073741824" | bc)
    
    success "ISO validated: ${ISO_SIZE_GB}GB"
    
    log "Generating checksums..."
    sha256sum "$ISO_PATH" > "$CHECKSUMS_FILE"
    md5sum "$ISO_PATH" >> "$CHECKSUMS_FILE"
    sha512sum "$ISO_PATH" >> "$CHECKSUMS_FILE"
    
    success "Checksums generated"
}

create_torrent() {
    log "Creating BitTorrent file..."
    
    if command -v mktorrent &> /dev/null; then
        mktorrent -a "http:
                  -a "http:
                  -c "OneOS v1.0 - Cross-device Operating System" \
                  -o "$TORRENT_DIR/oneos-1.0.torrent" \
                  "$ISO_PATH"
        
        success "Torrent created: $TORRENT_DIR/oneos-1.0.torrent"
    else
        log "mktorrent not found, creating metadata file instead..."
        
        cat > "$TORRENT_DIR/oneos-1.0.torrent.json" <<EOF
{
  "name": "oneos-1.0-final.iso",
  "length": $(stat -c%s "$ISO_PATH" 2>/dev/null || stat -f%z "$ISO_PATH" 2>/dev/null),
  "piece_length": 262144,
  "pieces": "binary_hash_array",
  "announce": "http:
  "announce-list": [
    ["http:
    ["http:
    ["http:
    ["https:
  ],
  "info": {
    "name": "oneos-1.0-final",
    "length": $(stat -c%s "$ISO_PATH" 2>/dev/null || stat -f%z "$ISO_PATH" 2>/dev/null),
    "piece_length": 262144
  }
}
EOF
        success "Torrent metadata created"
    fi
}

create_mirrors() {
    log "Setting up mirror distribution..."
    
    cp "$ISO_PATH" "$MIRROR_DIR/oneos-1.0-final.iso"
    cp "$CHECKSUMS_FILE" "$MIRROR_DIR/"
    
    success "ISO copied to mirrors directory"
    
    cat > "$MIRROR_DIR/MIRRORS.txt" <<'MIRRORS'

https:
https:

https:
http:
http:

https:
http:
http:

https:
http:
http:

magnet:?xt=urn:btih:[TORRENT_HASH]&dn=oneos-1.0-final.iso
MIRRORS
    
    success "Mirror list created"
}

create_download_page() {
    log "Generating download page..."
    
    cat > "releases/DOWNLOAD.html" <<'HTML'
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>OneOS v1.0 - Download</title>
    <style>
        body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif; 
               margin: 0; padding: 20px; background:
        .container { max-width: 1200px; margin: 0 auto; }
        .header { background:
                  text-align: center; margin-bottom: 40px; }
        .header h1 { margin: 0; font-size: 2.5em; }
        .header p { margin: 10px 0 0 0; color:
        .download-section { background: white; padding: 30px; border-radius: 8px; 
                           margin-bottom: 30px; box-shadow: 0 2px 8px rgba(0,0,0,0.1); }
        .download-option { margin: 20px 0; padding: 20px; border: 2px solid
                          border-radius: 5px; cursor: pointer; transition: all 0.3s; }
        .download-option:hover { border-color:
        .download-option.recommended { border-color:
        .download-option h3 { margin: 0 0 10px 0; color:
        .download-option p { margin: 5px 0; color:
        .btn { display: inline-block; padding: 10px 20px; background:
               color: white; text-decoration: none; border-radius: 4px; }
        .btn:hover { background:
        .info-box { background:
                   margin: 20px 0; border-radius: 4px; }
        .checksum { font-family: monospace; font-size: 0.85em; word-break: break-all; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>OneOS v1.0</h1>
            <p>Cross-device Operating System - Final Release</p>
        </div>
        
        <div class="download-section">
            <h2>Download Options</h2>
            
            <div class="download-option recommended">
                <h3>🔄 BitTorrent (Recommended)</h3>
                <p>Distributed peer-to-peer download - faster and more reliable for large files</p>
                <p><strong>Size:</strong> 4.2GB | <strong>Speed:</strong> Typically 5-10 MB/s</p>
                <a href="magnet:?xt=urn:btih:[HASH]&dn=oneos-1.0-final.iso" class="btn">Open Magnet Link</a>
                <a href="torrents/oneos-1.0.torrent" class="btn">Download .torrent File</a>
            </div>
            
            <div class="download-option">
                <h3>🌐 Direct Download - CDN</h3>
                <p>Global content delivery network - optimal for single connections</p>
                <p><strong>Speed:</strong> Up to 50 MB/s from nearest location</p>
                <a href="https:
            </div>
            
            <div class="download-option">
                <h3>🗺️ Regional Mirrors</h3>
                <p>Select a mirror closest to your location</p>
                <select onchange="window.location.href=this.value">
                    <option value="">Choose a mirror...</option>
                    <option value="https:
                    <option value="https:
                    <option value="https:
                </select>
            </div>
        </div>
        
        <div class="download-section">
            <h2>Verification</h2>
            <p>Always verify your download using SHA256:</p>
            <div class="checksum" style="background:
                sha256sum oneos-1.0-final.iso
                
                a1b2c3d4e5f6... (full hash)
            </div>
            <a href="CHECKSUMS.SHA256" class="btn">View All Checksums</a>
        </div>
        
        <div class="info-box">
            <strong>ℹ️ First Time?</strong>
            <p>See <a href="https:
        </div>
        
        <div class="download-section">
            <h2>System Requirements</h2>
            <ul>
                <li>4GB RAM minimum (8GB recommended)</li>
                <li>20GB disk space minimum (50GB recommended)</li>
                <li>Dual-core processor or better</li>
                <li>UEFI or Legacy BIOS boot support</li>
                <li>Ethernet or WiFi connection</li>
            </ul>
        </div>
        
        <div class="download-section">
            <h2>Version Info</h2>
            <p><strong>Version:</strong> 1.0 (Final Release)<br>
               <strong>Release Date:</strong> January 4, 2026<br>
               <strong>Support:</strong> 5-year LTS<br>
               <strong>License:</strong> Apache 2.0</p>
        </div>
    </div>
</body>
</html>
HTML
    
    success "Download page created"
}

create_manifest() {
    log "Creating release manifest..."
    
    cat > "releases/v1.0-MANIFEST.json" <<EOF
{
  "release": {
    "version": "1.0",
    "name": "OneOS v1.0 Final Release",
    "release_date": "$RELEASE_DATE",
    "type": "stable",
    "support_level": "LTS",
    "support_duration_years": 5
  },
  "files": {
    "iso": {
      "name": "oneos-1.0-final.iso",
      "size_bytes": $(stat -c%s "$ISO_PATH" 2>/dev/null || stat -f%z "$ISO_PATH" 2>/dev/null),
      "checksums": {
        "sha256": "$(sha256sum "$ISO_PATH" | awk '{print $1}')",
        "md5": "$(md5sum "$ISO_PATH" | awk '{print $1}')",
        "sha512": "$(sha512sum "$ISO_PATH" | awk '{print $1}')"
      }
    }
  },
  "distribution": {
    "torrent": "$TORRENT_DIR/oneos-1.0.torrent",
    "mirrors": [
      "https:
      "https:
      "https:
      "https:
    ]
  },
  "features": {
    "phases_complete": [
      "Phase 1: Installer System",
      "Phase 2: Application Store",
      "Phase 3: Security Framework",
      "Phase 4: Cross-Device Integration",
      "Phase 5: Testing & Validation"
    ],
    "key_features": [
      "KDE Connect clipboard sync",
      "LibrePods Bluetooth audio",
      "Element unified messaging",
      "Home Assistant smart home control",
      "PipeWire ultra-low latency audio",
      "WCAG 2.1 AA accessibility",
      "80% localization (15 languages)",
      "5-year LTS support"
    ]
  }
}
EOF
    
    success "Release manifest created"
}

validate_iso
create_torrent
create_mirrors
create_download_page
create_manifest

success "v1.0 Distribution System Ready"

