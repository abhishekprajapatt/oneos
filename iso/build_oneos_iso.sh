#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ONEOS_VERSION="2.1.0"
ONEOS_KERNEL="6.1.0"
ARCH="x86_64"

ISO_OUTPUT="${1:-OneOS-${ONEOS_VERSION}-${ARCH}.iso}"
WORK_DIR="${2:-/tmp/oneos-build}"

SQUASH_COMPRESSION="xz"
SQUASH_OPTIONS="-comp xz -Xbcj x86 -Xdict-size 100% -no-datablock-ordering"

ISO_LABEL="ONEOS"
ISO_PUBLISHER="OneOS Project"
ISO_PREPARER="OneOS Build System"

LOG_FILE="${WORK_DIR}/build.log"
VERBOSE=1

log() {
	echo "[$(date +'%Y-%m-%d %H:%M:%S')] $*" | tee -a "${LOG_FILE}"
}

error() {
	echo "[ERROR] $*" | tee -a "${LOG_FILE}" >&2
	exit 1
}

warn() {
	echo "[WARN] $*" | tee -a "${LOG_FILE}"
}

run_cmd() {
	local cmd="$*"
	log "Executing: $cmd"
	if eval "$cmd" >> "${LOG_FILE}" 2>&1; then
		return 0
	else
		error "Command failed: $cmd"
	fi
}

progress() {
	printf "%-60s" "$1"
	printf " [████████████████████████████████████████] 100%%\n"
}

check_prerequisites() {
	log "Checking prerequisites..."
	
	local tools=(
		"mkisofs" "xorriso" "squashfs-tools" "grub-mkimage"
		"find" "tar" "md5sum" "dd" "e2fsprogs"
	)
	
	for tool in "${tools[@]}"; do
		if ! command -v "$tool" &>/dev/null; then
			error "Required tool not found: $tool"
		fi
	done
	
	log "All prerequisites available"
}

check_space() {
	local required_space=$((20 * 1024 * 1024))
	local available_space=$(df "${WORK_DIR}" | awk 'NR==2 {print $4 * 1024}')
	
	if [ "$available_space" -lt "$required_space" ]; then
		error "Insufficient disk space. Required: 20 GB, Available: $((available_space / 1024 / 1024 / 1024)) GB"
	fi
	
	log "Disk space available: $((available_space / 1024 / 1024 / 1024)) GB"
}

setup_workspace() {
	log "Setting up build workspace..."
	
	mkdir -p "${WORK_DIR}"/{iso,boot,efi,live,modules}
	mkdir -p "${WORK_DIR}/iso/boot/grub"
	mkdir -p "${WORK_DIR}/iso/boot/isolinux"
	mkdir -p "${WORK_DIR}/iso/EFI/BOOT"
	mkdir -p "${WORK_DIR}/iso/live"
	
	progress "Creating directory structure"
	log "Workspace created at: ${WORK_DIR}"
}

setup_grub_bios() {
	log "Setting up GRUB BIOS bootloader..."
	
	mkdir -p "${WORK_DIR}/iso/boot/grub/i386-pc"
	run_cmd "cp /usr/lib/grub/i386-pc/*.mod ${WORK_DIR}/iso/boot/grub/i386-pc/"
	
	if [ -f "/usr/lib/grub/i386-pc/boot.img" ]; then
		cp "/usr/lib/grub/i386-pc/boot.img" "${WORK_DIR}/boot/"
	fi
	
	progress "GRUB BIOS bootloader configured"
}

setup_grub_efi() {
	log "Setting up GRUB UEFI bootloader..."
	
	mkdir -p "${WORK_DIR}/iso/EFI/BOOT"
	
	run_cmd "grub-mkimage \
		-o ${WORK_DIR}/iso/EFI/BOOT/BOOTX64.EFI \
		-O x86_64-efi \
		-p /EFI/BOOT \
		--prefix='(memdisk)/boot/grub' \
		search iso9660 normal configfile"
	
	mkdir -p "${WORK_DIR}/iso/boot/grub/x86_64-efi"
	run_cmd "cp /usr/lib/grub/x86_64-efi/*.mod ${WORK_DIR}/iso/boot/grub/x86_64-efi/"
	
	progress "GRUB UEFI bootloader configured"
}

setup_isolinux() {
	log "Setting up ISOLINUX legacy bootloader..."
	
	if [ -f "/usr/lib/syslinux/isolinux.bin" ]; then
		cp "/usr/lib/syslinux/isolinux.bin" "${WORK_DIR}/iso/boot/isolinux/"
	fi
	
	if [ -f "/usr/lib/syslinux/ldlinux.c32" ]; then
		cp "/usr/lib/syslinux/ldlinux.c32" "${WORK_DIR}/iso/boot/isolinux/"
	fi
	
	progress "ISOLINUX bootloader configured"
}

copy_bootloader_configs() {
	log "Copying bootloader configurations..."
	
	if [ -f "${SCRIPT_DIR}/grub.cfg" ]; then
		cp "${SCRIPT_DIR}/grub.cfg" "${WORK_DIR}/iso/boot/grub/"
	fi
	
	if [ -f "${SCRIPT_DIR}/isolinux.cfg" ]; then
		cp "${SCRIPT_DIR}/isolinux.cfg" "${WORK_DIR}/iso/boot/isolinux/"
	fi
	
	progress "Bootloader configurations copied"
}

copy_kernel_initramfs() {
	log "Copying kernel and initramfs..."
	
	mkdir -p "${WORK_DIR}/iso/live"
	
	if [ -f "/boot/vmlinuz-linux" ]; then
		cp "/boot/vmlinuz-linux" "${WORK_DIR}/iso/live/vmlinuz-linux"
		log "Kernel copied: vmlinuz-linux"
	else
		warn "Kernel not found at /boot/vmlinuz-linux"
	fi
	
	if [ -f "/boot/initramfs-linux.img" ]; then
		cp "/boot/initramfs-linux.img" "${WORK_DIR}/iso/live/"
		log "Initramfs copied: initramfs-linux.img"
	else
		warn "Initramfs not found at /boot/initramfs-linux.img"
	fi
	
	if [ -f "/boot/memtest.bin" ]; then
		cp "/boot/memtest.bin" "${WORK_DIR}/iso/live/"
	fi
	
	progress "Kernel and initramfs ready"
}

create_rootfs() {
	log "Creating root filesystem image..."
	
	local rootfs_dir="${WORK_DIR}/rootfs"
	mkdir -p "${rootfs_dir}"
	
	log "Populating root filesystem..."
	
	mkdir -p "${rootfs_dir}"/{bin,sbin,lib,lib64,etc,usr,var,tmp,proc,sys,dev,mnt,opt,root,home}
	
	[ -d "/bin" ] && cp -r /bin/* "${rootfs_dir}/bin/" 2>/dev/null || true
	[ -d "/sbin" ] && cp -r /sbin/* "${rootfs_dir}/sbin/" 2>/dev/null || true
	[ -d "/lib" ] && cp -r /lib/* "${rootfs_dir}/lib/" 2>/dev/null || true
	[ -d "/etc" ] && cp -r /etc/* "${rootfs_dir}/etc/" 2>/dev/null || true
	[ -d "/usr" ] && cp -r /usr/* "${rootfs_dir}/usr/" 2>/dev/null || true
	
	log "Creating Squashfs filesystem..."
	run_cmd "mksquashfs ${rootfs_dir} ${WORK_DIR}/iso/live/filesystem.squashfs ${SQUASH_OPTIONS}"
	
	rm -rf "${rootfs_dir}"
	
	progress "Root filesystem created"
}

create_iso_image() {
	log "Creating ISO image..."
	
	local iso_cmd=(
		"xorriso"
		"-as" "mkisofs"
		"-iso-level" "3"
		"-full-iso9660-filenames"
		"-volid" "'${ISO_LABEL}'"
		"-publisher" "'${ISO_PUBLISHER}'"
		"-preparer" "'${ISO_PREPARER}'"
		"-r" "-R" "-J"
		
		"-isohybrid-mbr" "/usr/lib/syslinux/isohdpfx.bin"
		"-c" "isolinux/boot.cat"
		"-b" "isolinux/isolinux.bin"
		"-no-emul-boot"
		"-boot-load-size" "4"
		"-boot-info-table"
		
		"-eltorito-alt-boot"
		"-e" "EFI/efiboot.img"
		"-no-emul-boot"
		"-isohybrid-gpt-basdat"
		
		"-output" "'${ISO_OUTPUT}'"
		"'${WORK_DIR}/iso'"
	)
	
	eval "${iso_cmd[@]}"
	
	progress "ISO image created"
	log "ISO created: ${ISO_OUTPUT}"
}

verify_iso() {
	log "Verifying ISO image..."
	
	if [ ! -f "${ISO_OUTPUT}" ]; then
		error "ISO file not found: ${ISO_OUTPUT}"
	fi
	
	local iso_size=$(du -h "${ISO_OUTPUT}" | cut -f1)
	log "ISO size: ${iso_size}"
	
	local checksum=$(md5sum "${ISO_OUTPUT}" | cut -d' ' -f1)
	log "MD5 checksum: ${checksum}"
	
	log "ISO contents:"
	run_cmd "isoinfo -R -f -i ${ISO_OUTPUT} | head -20"
	
	progress "ISO verification complete"
}

create_efiboot_image() {
	log "Creating EFI boot image..."
	
	local efiboot="${WORK_DIR}/efiboot.img"
	
	dd if=/dev/zero of="${efiboot}" bs=1M count=200
	
	mkfs.vfat -n EFIBOOT "${efiboot}"
	
	local mount_point="${WORK_DIR}/efiboot_mount"
	mkdir -p "${mount_point}"
	sudo mount -o loop "${efiboot}" "${mount_point}"
	
	mkdir -p "${mount_point}/EFI/BOOT"
	cp "${WORK_DIR}/iso/EFI/BOOT/BOOTX64.EFI" "${mount_point}/EFI/BOOT/"
	
	sudo umount "${mount_point}"
	rm -rf "${mount_point}"
	
	cp "${efiboot}" "${WORK_DIR}/iso/EFI/efiboot.img"
	
	progress "EFI boot image created"
}

main() {
	log "=========================================="
	log "OneOS ISO Builder v${ONEOS_VERSION}"
	log "=========================================="
	log "Output: ${ISO_OUTPUT}"
	log "Work directory: ${WORK_DIR}"
	log ""
	
	check_prerequisites
	check_space
	setup_workspace
	
	setup_grub_bios
	setup_grub_efi
	setup_isolinux
	copy_bootloader_configs
	
	copy_kernel_initramfs
	
	create_efiboot_image
	
	create_rootfs
	
	create_iso_image
	
	verify_iso
	
	log "=========================================="
	log "ISO build completed successfully!"
	log "Output: ${ISO_OUTPUT}"
	log "=========================================="
}

main "$@"

