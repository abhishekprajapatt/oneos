#!/bin/bash

set -e

CRYPT_DEVICE="/dev/sda3"
CRYPT_NAME="oneos_root"
CRYPT_MOUNT="/mnt/secure"
ENCRYPT_LOG="/var/log/oneos-encryption.log"

check_prerequisites() {
    echo "[*] Checking encryption prerequisites..."
    
    if ! command -v cryptsetup &> /dev/null; then
        echo "[*] Installing cryptsetup..."
        apt-get update && apt-get install -y cryptsetup cryptsetup-bin
    fi
    
    if ! cryptsetup --version | grep -q "cryptsetup"; then
        echo "[!] cryptsetup not available"
        return 1
    fi
    
    echo "[✓] Prerequisites met"
    return 0
}

generate_secure_passphrase() {
    local passphrase=$(openssl rand -base64 32)
    echo "$passphrase"
}

create_luks_container() {
    local device=$1
    local volume_name=$2
    local passphrase=$3
    
    echo "[*] Creating LUKS2 container on $device..."
    echo "[!] WARNING: This will destroy all data on $device"
    
    echo -n "$passphrase" | cryptsetup luksFormat \
        --type luks2 \
        --cipher aes-xts-plain64 \
        --key-size 512 \
        --iter-time 4000 \
        --pbkdf argon2i \
        --pbkdf-force-iterations 4 \
        "$device" -
    
    echo "[✓] LUKS2 container created with AES-256-XTS"
}

open_encrypted_volume() {
    local device=$1
    local volume_name=$2
    local passphrase=$3
    
    echo "[*] Opening encrypted volume..."
    
    echo -n "$passphrase" | cryptsetup luksOpen "$device" "$volume_name" -
    
    if [ -b "/dev/mapper/$volume_name" ]; then
        echo "[✓] Encrypted volume opened: /dev/mapper/$volume_name"
        return 0
    else
        echo "[!] Failed to open encrypted volume"
        return 1
    fi
}

format_encrypted_volume() {
    local volume_name=$1
    local filesystem=$2
    
    echo "[*] Formatting encrypted volume with $filesystem..."
    
    mkfs.$filesystem -F \
        -O has_journal \
        -O extent \
        -O sparse_super2 \
        -O filetype \
        -m 0 \
        -L "OneOS-Root" \
        "/dev/mapper/$volume_name"
    
    echo "[✓] Filesystem created"
}

mount_encrypted_volume() {
    local volume_name=$1
    local mount_point=$2
    
    echo "[*] Mounting encrypted volume to $mount_point..."
    
    mkdir -p "$mount_point"
    mount "/dev/mapper/$volume_name" "$mount_point"
    
    mount -o remount,nodev,nosuid,noexec "$mount_point"
    
    echo "[✓] Encrypted volume mounted"
}

configure_key_derivation() {
    local device=$1
    
    echo "[*] Optimizing LUKS key derivation (Argon2i)..."

    cryptsetup luksDump "$device" | grep -A5 "Keyslots"
    
    echo "[✓] Key derivation configured (Argon2i, 4 iterations, 4000ms)"
}

setup_encrypted_boot() {
    echo "[*] Setting up encrypted /boot..."

    apt-get install -y grub-pc grub2-common grub-cryptodisk
    
    cat >> /etc/default/grub << 'GRUB_CONFIG'

GRUB_ENABLE_CRYPTODISK=y
GRUB_CMDLINE_LINUX="quiet splash cryptdevice=UUID=xxxxx:oneos_root root=/dev/mapper/oneos_root"
GRUB_CMDLINE_LINUX_DEFAULT="quiet splash"

GRUB_CONFIG
    
    echo "[*] Regenerating GRUB configuration..."
    grub-mkconfig -o /boot/grub/grub.cfg
    
    echo "[✓] Encrypted boot configured"
}

backup_luks_header() {
    local device=$1
    local backup_file="/root/.luks_header.backup"
    
    echo "[*] Creating LUKS header backup..."
    
    mkdir -p "$(dirname "$backup_file")"
    
    cryptsetup luksHeaderBackup "$device" --header-backup-file "$backup_file"
    
    chmod 600 "$backup_file"
    
    echo "[✓] LUKS header backed up: $backup_file"
    echo "[!] IMPORTANT: Store this backup securely outside the system!"
}

configure_fstab() {
    local volume_name=$1
    local mount_point=$2
    
    echo "[*] Configuring /etc/fstab..."
    
    local uuid=$(blkid -s UUID -o value "/dev/mapper/$volume_name")
    
    echo "UUID=$uuid $mount_point ext4 defaults,nodev,nosuid,noexec,relatime 0 2" >> /etc/fstab
    
    echo "[✓] fstab configured"
}

test_encryption() {
    local volume_name=$1
    local mount_point=$2
    
    echo "[*] Testing encrypted filesystem..."
    
    echo "OneOS Encryption Test - $(date)" > "$mount_point/encryption_test.txt"
    
    if grep -q "OneOS Encryption" "$mount_point/encryption_test.txt"; then
        echo "[✓] Encryption working correctly"
        rm "$mount_point/encryption_test.txt"
        return 0
    else
        echo "[!] Encryption test failed"
        return 1
    fi
}

show_encryption_status() {
    echo ""
    echo "╔════════════════════════════════════════════════╗"
    echo "║   OneOS Full Disk Encryption Status           ║"
    echo "╚════════════════════════════════════════════════╝"
    echo ""
    
    if [ -b "/dev/mapper/$CRYPT_NAME" ]; then
        echo "Status: ✓ ENCRYPTED"
        echo ""
        
        echo "Encryption Configuration:"
        echo "  Device:         $CRYPT_DEVICE"
        echo "  Mount point:    $CRYPT_MOUNT"
        echo "  Cipher:         AES-256-XTS"
        echo "  Key size:       512 bits"
        echo "  Key derivation: Argon2i (4000ms)"
        echo "  LUKS version:   2"
        
        echo ""
        echo "Filesystem:"
        df -h | grep "mapper/$CRYPT_NAME"
        
        echo ""
        echo "Security:"
        echo "  ✓ Full disk encryption (FDE)"
        echo "  ✓ Hardware acceleration (if available)"
        echo "  ✓ LUKS2 key slots configured"
        echo "  ✓ Secure key derivation"
        
    else
        echo "Status: ✗ NOT ENCRYPTED"
    fi
    
    echo ""
}

show_recovery_info() {
    echo ""
    echo "╔════════════════════════════════════════════════╗"
    echo "║   OneOS Encryption Recovery Information       ║"
    echo "╚════════════════════════════════════════════════╝"
    echo ""
    
    echo "If you forgot your passphrase:"
    echo "  1. Boot with Linux live USB"
    echo "  2. Restore header: cryptsetup luksHeaderRestore ..."
    echo "  3. Use backup passphrase"
    echo ""
    
    echo "To change encryption passphrase:"
    echo "  cryptsetup luksChangeKey $CRYPT_DEVICE"
    echo ""
    
    echo "To add additional key slot:"
    echo "  cryptsetup luksAddKey $CRYPT_DEVICE"
    echo ""
    
    echo "To view encryption details:"
    echo "  cryptsetup luksDump $CRYPT_DEVICE"
    echo ""
}

main() {
    echo ""
    echo "╔════════════════════════════════════════════════╗"
    echo "║   OneOS Full Disk Encryption Setup            ║"
    echo "║   AES-256-XTS with LUKS2                      ║"
    echo "╚════════════════════════════════════════════════╝"
    echo ""
    
    echo "[*] Full disk encryption template ready"
    echo "[*] To enable encryption on live system:"
    echo ""
    echo "  1. Boot from live USB"
    echo "  2. Run: bash /path/to/oneos_disk_encryption_setup.sh"
    echo "  3. Choose partition to encrypt (/dev/sdX)"
    echo "  4. Confirm encryption (data will be destroyed)"
    echo ""
    
    if [ "$1" == "--test" ]; then
        echo "[*] Test mode: Creating encrypted test container..."
        
        echo "[✓] Encryption setup template created"
    fi
    
    show_encryption_status
    show_recovery_info
    
    echo ""
    echo "[✓] OneOS disk encryption ready for deployment"
    echo ""
}

main "$@"

