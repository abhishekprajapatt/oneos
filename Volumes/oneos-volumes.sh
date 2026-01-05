#!/bin/bash

VOLUMES_DIR="/Volumes"

volumes_init() {
    mkdir -p "$VOLUMES_DIR"
    mkdir -p "$VOLUMES_DIR/MacintoshHD"
    mkdir -p "$VOLUMES_DIR/.devices"
    
    echo "✓ Volumes system initialized"
}

volume_create() {
    local name=$1
    local mount_point=$2
    local type=${3:-1}
    
    if [ -z "$name" ] || [ -z "$mount_point" ]; then
        echo "Usage: $0 volume-create <name> <mount_point> [type]"
        return 1
    fi
    
    mkdir -p "$VOLUMES_DIR/$name"
    
    cat > "$VOLUMES_DIR/$name/.volume.info" << EOF
name=$name
uuid=$name-$(date +%s)
mount_point=$mount_point
type=$type
state=0
filesystem=ext4
created=$(date +%s)
total_size=0
used_space=0
read_only=0
EOF
    
    echo "✓ Volume created: $name"
}

volume_mount() {
    local name=$1
    local mount_point=$2
    
    if [ -z "$name" ] || [ -z "$mount_point" ]; then
        echo "Usage: $0 volume-mount <name> <mount_point>"
        return 1
    fi
    
    if [ -f "$VOLUMES_DIR/$name/.volume.info" ]; then
        sed -i "s/state=.*/state=1/" "$VOLUMES_DIR/$name/.volume.info"
        sed -i "s|mount_point=.*|mount_point=$mount_point|" "$VOLUMES_DIR/$name/.volume.info"
        echo "✓ Volume mounted: $name at $mount_point"
    else
        echo "Volume not found: $name"
        return 1
    fi
}

volume_unmount() {
    local name=$1
    
    if [ -z "$name" ]; then
        echo "Usage: $0 volume-unmount <name>"
        return 1
    fi
    
    if [ -f "$VOLUMES_DIR/$name/.volume.info" ]; then
        sed -i "s/state=.*/state=0/" "$VOLUMES_DIR/$name/.volume.info"
        echo "✓ Volume unmounted: $name"
    else
        echo "Volume not found: $name"
        return 1
    fi
}

volume_list() {
    echo "Mounted Volumes:"
    echo "=========================================="
    
    for vol_dir in "$VOLUMES_DIR" 2>/dev/null | grep -v ".devices" | wc -l)"
    echo "Storage Devices: $(ls -1 "$VOLUMES_DIR/.devices"/*.dev 2>/dev/null | wc -l)"
    echo ""
}

show_help() {
    cat << 'EOF'
OneOS Volumes Management Script

Usage: oneos-volumes <command> [options]

Volume Commands:
  volume-create <name> <mp> [type]    Create volume
  volume-mount <name> <mp>            Mount volume
  volume-unmount <name>               Unmount volume
  volume-list                         List all volumes

Storage Commands:
  storage-create <device> <size>      Create storage device
  storage-list                        List storage devices

Partition Commands:
  partition-create <dev> <name> <sz> [fs]  Create partition
  partition-list <device>             List partitions

MacintoshHD Commands:
  macintosh-hd-info                   Show volume information

System Commands:
  init                                Initialize volumes system
  status                              Show system status
  help                                Show this help message

Examples:
  oneos-volumes init
  oneos-volumes volume-create MyVolume /mnt/myvolume
  oneos-volumes storage-create sda 1099511627776
  oneos-volumes partition-create sda root 536870912
  oneos-volumes partition-list sda
  oneos-volumes volume-list
  oneos-volumes status
EOF
}

case "${1:-help}" in
    init)
        volumes_init
        ;;
    volume-create)
        volume_create "$2" "$3" "$4"
        ;;
    volume-mount)
        volume_mount "$2" "$3"
        ;;
    volume-unmount)
        volume_unmount "$2"
        ;;
    volume-list)
        volume_list
        ;;
    storage-create)
        storage_create "$2" "$3"
        ;;
    storage-list)
        storage_list
        ;;
    partition-create)
        partition_create "$2" "$3" "$4" "$5"
        ;;
    partition-list)
        partition_list "$2"
        ;;
    macintosh-hd-info)
        macintosh_hd_info
        ;;
    status)
        status
        ;;
    help|*)
        show_help
        ;;
esac

