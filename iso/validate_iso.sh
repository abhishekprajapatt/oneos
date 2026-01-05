#!/bin/bash

set -e

ISO_FILE="${1:-.}"
TEMP_DIR="/tmp/oneos-iso-check"
LOG_FILE="${TEMP_DIR}/validation.log"

log() {
	echo "[$(date +'%H:%M:%S')] $*" | tee -a "${LOG_FILE}"
}

error() {
	echo "[ERROR] $*" | tee -a "${LOG_FILE}" >&2
	exit 1
}

check_file() {
	local file=$1
	if [ ! -f "$file" ]; then
		error "File not found: $file"
	fi
	log "✓ File found: $file"
}

validate_iso() {
	log "=================================="
	log "OneOS ISO Validation v2.1.0"
	log "=================================="
	log ""
	
	if [ -d "$ISO_FILE" ]; then
		log "Scanning directory: $ISO_FILE"
		local iso_count=$(find "$ISO_FILE" -maxdepth 1 -name "*.iso" | wc -l)
		log "Found $iso_count ISO files"
		return
	fi
	
	check_file "$ISO_FILE"
	
	local iso_size=$(du -h "$ISO_FILE" | cut -f1)
	log "ISO size: $iso_size"
	
	local file_type=$(file -b "$ISO_FILE")
	log "File type: $file_type"
	
	log "Computing MD5 checksum..."
	local md5=$(md5sum "$ISO_FILE" | cut -d' ' -f1)
	log "MD5: $md5"
	
	log ""
	log "ISO Content Validation:"
	log "========================"
	
	validate_bootloader
	
	validate_kernel
	
	validate_filesystem
	
	validate_integrity
	
	log ""
	log "=================================="
	log "Validation Complete"
	log "=================================="
}

validate_bootloader() {
	log "Checking bootloaders..."
	
	if isoinfo -R -f -i "$ISO_FILE" 2>/dev/null | grep -q "boot/grub"; then
		log "✓ GRUB bootloader found"
	else
		log "⚠ GRUB bootloader not found"
	fi
	
	if isoinfo -R -f -i "$ISO_FILE" 2>/dev/null | grep -q "isolinux.bin"; then
		log "✓ ISOLINUX bootloader found"
	else
		log "⚠ ISOLINUX bootloader not found"
	fi
	
	if isoinfo -R -f -i "$ISO_FILE" 2>/dev/null | grep -q "BOOTX64.EFI"; then
		log "✓ UEFI bootloader (BOOTX64.EFI) found"
	else
		log "⚠ UEFI bootloader not found"
	fi
	
	if isoinfo -R -f -i "$ISO_FILE" 2>/dev/null | grep -q "grub.cfg"; then
		log "✓ GRUB configuration found"
	else
		log "⚠ GRUB configuration not found"
	fi
}

validate_kernel() {
	log "Checking kernel and initramfs..."
	
	if isoinfo -R -f -i "$ISO_FILE" 2>/dev/null | grep -q "vmlinuz"; then
		log "✓ Kernel image found"
		local kernel_info=$(isoinfo -R -f -i "$ISO_FILE" 2>/dev/null | grep vmlinuz)
		log "  Kernel: $kernel_info"
	else
		log "⚠ Kernel image not found"
	fi
	
	if isoinfo -R -f -i "$ISO_FILE" 2>/dev/null | grep -q "initramfs"; then
		log "✓ Initramfs found"
		local initrd_info=$(isoinfo -R -f -i "$ISO_FILE" 2>/dev/null | grep initramfs)
		log "  Initramfs: $initrd_info"
	else
		log "⚠ Initramfs not found"
	fi
}

validate_filesystem() {
	log "Checking filesystem..."
	
	if isoinfo -R -f -i "$ISO_FILE" 2>/dev/null | grep -q "squashfs"; then
		log "✓ Squashfs filesystem found"
		local fs_size=$(isoinfo -R -l -i "$ISO_FILE" 2>/dev/null | grep squashfs | awk '{print $5}')
		log "  Filesystem size: $(numfmt --to=iec $fs_size 2>/dev/null || echo "$fs_size bytes")"
	else
		log "⚠ Squashfs filesystem not found"
	fi
	
	local dirs=("boot" "EFI" "live")
	for dir in "${dirs[@]}"; do
		if isoinfo -R -f -i "$ISO_FILE" 2>/dev/null | grep -q "/$dir"; then
			log "✓ Directory /$dir found"
		else
			log "⚠ Directory /$dir not found"
		fi
	done
}

validate_integrity() {
	log "Checking ISO integrity..."
	
	local mount_point="/tmp/oneos-iso-mount"
	mkdir -p "$mount_point"
	
	if sudo mount -o loop,ro "$ISO_FILE" "$mount_point" 2>/dev/null; then
		log "✓ ISO mounted successfully"
		
		if [ -f "$mount_point/boot/grub/grub.cfg" ]; then
			log "✓ GRUB configuration accessible"
		fi
		
		if [ -f "$mount_point/live/vmlinuz-linux" ]; then
			log "✓ Kernel accessible"
		fi
		
		sudo umount "$mount_point"
	else
		log "⚠ Could not mount ISO for integrity check"
	fi
	
	rm -rf "$mount_point"
}

mkdir -p "$TEMP_DIR"
validate_iso

