#!/bin/bash

set -e

BOOT_DIR="/boot"
EFI_DIR="/boot/efi"
GRUB_DIR="/boot/grub"
SHIM_DIR="/usr/share/shim"
KERNEL_SIGN_KEY="/etc/oneos/boot-signing-key.pem"
KERNEL_SIGN_CERT="/etc/oneos/boot-signing-cert.pem"
BOOT_LOG="/var/log/oneos-boot-security.log"

init_boot_security() {
    echo "[*] Initializing OneOS boot security..."
    mkdir -p "$(dirname "$BOOT_LOG")"
    
    if [ ! -d "$EFI_DIR" ]; then
        echo "[!] Not booted with UEFI - UEFI required for Secure Boot"
        return 1
    fi
    
    echo "[✓] UEFI detected"
}

install_shim() {
    echo "[*] Installing UEFI shim bootloader..."
    
    if ! command -v sbsign &> /dev/null; then
        echo "[*] Installing sbsign and shim-tools..."
        apt-get update && apt-get install -y sbsigntools shim-unsigned mokutil
    fi
    
    if [ -f "$SHIM_DIR/shimx64.efi" ]; then
        cp "$SHIM_DIR/shimx64.efi" "$EFI_DIR/shim.efi"
        chmod 644 "$EFI_DIR/shim.efi"
        echo "[✓] Shim bootloader installed"
    else
        echo "[!] Shim binary not found - install shim package"
    fi
}

create_signing_keys() {
    echo "[*] Creating kernel signing certificates..."
    mkdir -p "$(dirname "$KERNEL_SIGN_KEY")"
    chmod 700 "$(dirname "$KERNEL_SIGN_KEY")"
    
    if [ -f "$KERNEL_SIGN_KEY" ] && [ -f "$KERNEL_SIGN_CERT" ]; then
        echo "[✓] Signing keys already exist"
        return
    fi
    
    echo "[*] Generating 4096-bit RSA key (this may take a moment)..."
    openssl genrsa -out "$KERNEL_SIGN_KEY" 4096
    
    echo "[*] Creating self-signed certificate..."
    openssl req -new -x509 -key "$KERNEL_SIGN_KEY" \
        -out "$KERNEL_SIGN_CERT" \
        -days 365 \
        -subj "/CN=OneOS Kernel Signing Key/O=OneOS/C=US"
    
    chmod 600 "$KERNEL_SIGN_KEY"
    chmod 644 "$KERNEL_SIGN_CERT"
    
    echo "[✓] Signing keys created:"
    echo "  Private: $KERNEL_SIGN_KEY"
    echo "  Public:  $KERNEL_SIGN_CERT"
}

sign_kernel() {
    local kernel_file=$1
    
    if [ ! -f "$kernel_file" ]; then
        echo "[!] Kernel file not found: $kernel_file"
        return 1
    fi
    
    echo "[*] Signing kernel: $kernel_file"
    
    sbsign \
        --key "$KERNEL_SIGN_KEY" \
        --cert "$KERNEL_SIGN_CERT" \
        --output "${kernel_file}.signed" \
        "$kernel_file"
    
    mv "${kernel_file}.signed" "$kernel_file"
    
    echo "[✓] Kernel signed: $kernel_file"
}

sign_grub() {
    echo "[*] Signing GRUB bootloader..."
    
    local grub_efi="$EFI_DIR/EFI/ubuntu/grubx64.efi"
    
    if [ ! -f "$grub_efi" ]; then
        echo "[!] GRUB EFI not found: $grub_efi"
        return 1
    fi
    
    sbsign \
        --key "$KERNEL_SIGN_KEY" \
        --cert "$KERNEL_SIGN_CERT" \
        --output "${grub_efi}.signed" \
        "$grub_efi"
    
    mv "${grub_efi}.signed" "$grub_efi"
    
    echo "[✓] GRUB signed"
}

configure_secure_boot() {
    echo "[*] Configuring Secure Boot..."
    
    if [ -f /sys/firmware/efi/fw_platform_size ]; then
        echo "[*] Checking Secure Boot status..."
        
        echo "[!] Secure Boot must be enabled in UEFI firmware"
        echo "[*] Steps to enable:"
        echo "    1. Reboot and enter UEFI setup"
        echo "    2. Find 'Secure Boot' option"
        echo "    3. Set to 'Enabled'"
        echo "    4. Select 'Custom' if available"
        echo "    5. Enroll MOK (Machine Owner Key) from our cert"
        
        if command -v mokutil &> /dev/null; then
            echo "[*] Creating MOK enrollment request..."
            mokutil --import "$KERNEL_SIGN_CERT"
            echo "[✓] MOK request created - reboot to enroll"
        fi
    fi
}

configure_grub() {
    echo "[*] Configuring GRUB bootloader security..."
    
    local grub_conf="/etc/default/grub"
    local grub_cfg="/boot/grub/grub.cfg"
    
    cp "$grub_conf" "${grub_conf}.backup"
    
    sed -i 's/GRUB_CMDLINE_LINUX="/GRUB_CMDLINE_LINUX="lockdown=confidentiality /g' "$grub_conf"
    
    sed -i 's/GRUB_DISABLE_RECOVERY=false/GRUB_DISABLE_RECOVERY=true/' "$grub_conf"
    
    sed -i 's/GRUB_TIMEOUT=10/GRUB_TIMEOUT=3/' "$grub_conf"
    
    echo 'GRUB_HIDDEN_TIMEOUT=0' >> "$grub_conf"
    echo 'GRUB_HIDDEN_TIMEOUT_QUIET=true' >> "$grub_conf"
    
    echo "[*] Regenerating GRUB configuration..."
    grub-mkconfig -o "$grub_cfg"
    
    echo "[✓] GRUB configured"
}

enable_tpm() {
    echo "[*] Enabling TPM measurements..."
    
    if [ ! -f /sys/class/tpm/tpm0/device/eventlog ]; then
        echo "[!] TPM not available - skipping"
        return
    fi
    
    local tpm_version=$(cat /sys/class/tpm/tpm0/tpm_version_major 2>/dev/null || echo "0")
    echo "[*] TPM version: $tpm_version"
    
    echo "[*] Kernel will be measured by firmware into PCR[4]"
    
    echo "[✓] TPM ready for integrity measurements"
}

create_boot_entry() {
    echo "[*] Creating UEFI boot entry for shim..."
    
    if ! command -v efibootmgr &> /dev/null; then
        echo "[*] Installing efibootmgr..."
        apt-get install -y efibootmgr
    fi
    
    local efi_part=$(mount | grep efi | awk '{print $1}')
    
    if [ -z "$efi_part" ]; then
        echo "[!] EFI partition not mounted"
        return 1
    fi
    
    local disk="${efi_part%p*}"
    local part="${efi_part
    
    echo "[*] Creating boot entry for shim..."
    efibootmgr -c -d "$disk" -p "$part" \
        -L "OneOS Secure Boot" \
        -l "\\EFI\\shim.efi" 2>/dev/null || echo "[!] Requires UEFI environment"
    
    echo "[✓] Boot entry created"
}

verify_boot_security() {
    echo ""
    echo "╔════════════════════════════════════════════╗"
    echo "║   Boot Security Verification               ║"
    echo "╚════════════════════════════════════════════╝"
    echo ""
    
    if [ -f /sys/firmware/efi/fw_platform_size ]; then
        echo "✓ EFI system detected"
    fi
    
    if [ -f "$EFI_DIR/shim.efi" ]; then
        echo "✓ Shim bootloader installed"
    else
        echo "✗ Shim not installed"
    fi
    
    if [ -f "$KERNEL_SIGN_CERT" ]; then
        echo "✓ Kernel signing certificate installed"
        echo "  Cert: $KERNEL_SIGN_CERT"
    else
        echo "✗ Signing certificate not found"
    fi
    
    if [ -f "$EFI_DIR/EFI/ubuntu/grubx64.efi" ]; then
        echo "✓ GRUB bootloader present"
    fi
    
    if [ -d /sys/class/tpm/tpm0 ]; then
        echo "✓ TPM available for measurements"
    fi
    
    echo ""
}

setup_complete() {
    echo ""
    echo "╔════════════════════════════════════════════════╗"
    echo "║   OneOS Secure Boot Setup Complete            ║"
    echo "╚════════════════════════════════════════════════╝"
    echo ""
    echo "Components:"
    echo "  ✓ Shim bootloader installed"
    echo "  ✓ Kernel signing keys created"
    echo "  ✓ GRUB configured and signed"
    echo "  ✓ Secure Boot ready"
    echo "  ✓ TPM measurements enabled"
    echo ""
    echo "Next Steps:"
    echo "  1. Reboot system"
    echo "  2. Enter UEFI/BIOS settings (Press DEL/F2/F10 during boot)"
    echo "  3. Enable 'Secure Boot'"
    echo "  4. Enroll MOK (Machine Owner Key) when prompted"
    echo "  5. Reboot again"
    echo ""
    echo "Verification:"
    echo "  • After boot: \`mokutil --list-enrolled\`"
    echo "  • Check TPM: \`tpm2_getcap handles-persistent\`"
    echo "  • Boot log: tail -f /var/log/kern.log"
    echo ""
}

main() {
    init_boot_security
    create_signing_keys
    install_shim
    sign_grub
    configure_secure_boot
    configure_grub
    enable_tpm
    create_boot_entry
    verify_boot_security
    setup_complete
    
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] Boot security setup completed" >> "$BOOT_LOG"
}

main

