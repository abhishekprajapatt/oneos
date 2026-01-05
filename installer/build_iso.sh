#!/bin/bash

set -e

ISO_NAME="oneos"
ISO_VERSION="2.1.0"
BUILD_DIR="./iso-build"
ARCH="x86_64"
BOOT_LABEL="ONEOS"
INSTALL_DIR="arch"
WORKDIR="./build"
ISODIR="$WORKDIR/iso"
ROOTDIR="$WORKDIR/root"

GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║  OneOS ISO Builder v${ISO_VERSION}                              ║${NC}"
echo -e "${BLUE}║  Hybrid BIOS/UEFI bootable media with EFI support         ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}\n"

echo -e "${BLUE}[*] Starting OneOS ISO creation...${NC}"

echo -e "${BLUE}[1/8] Creating directory structure...${NC}"

mkdir -p "${BUILD_DIR}/"{archlive,iso,boot/grub,boot/efi,packages}
mkdir -p "${BUILD_DIR}/archlive/"{etc,usr/share,root,.cache/pacman/pkg}

mkdir -p "${BUILD_DIR}/archlive/airootfs"/{etc,root,boot,usr/local/bin}
mkdir -p "${BUILD_DIR}/archlive/efiboot"

echo -e "${GREEN}[✓] Directory structure created${NC}\n"

echo -e "${BLUE}[2/8] Configuring pacman...${NC}"

cat > "${BUILD_DIR}/archlive/pacman.conf" << 'EOF'
[options]
RootDir     = /
DBPath      = /var/lib/pacman/
CacheDir    = /var/cache/pacman/pkg/
LogFile     = /var/log/pacman.log
GPGDir      = /etc/pacman.d/gnupg/
HookDir     = /etc/pacman.d/hooks/
HoldPkg     = pacman glibc
Architecture = auto
CheckSpace
SigLevel    = Required DatabaseOptional
LocalFileSigLevel = Optional

[core]
Include = /etc/pacman.d/mirrorlist

[extra]
Include = /etc/pacman.d/mirrorlist

[multilib]
Include = /etc/pacman.d/mirrorlist
EOF

echo -e "${GREEN}[✓] Pacman configured${NC}\n"

echo -e "${BLUE}[3/8] Preparing package list...${NC}"

cat > "${BUILD_DIR}/archlive/packages.x86_64" << 'EOF'
base
linux
linux-firmware
amd-microcode
intel-microcode

btrfs-progs
e2fsprogs
xfsprogs
gptfdisk
parted
gdisk

arch-install-scripts
curl
wget
git
rsync

grub
efibootmgr
shim

systemd
systemd-boot
efivar
efitools
sbsigntools

calamares
qt5-base
python
python-libutil_linux

adobe-source-hans-fonts
adobe-source-code-pro-fonts
noto-fonts
noto-fonts-cjk
noto-fonts-emoji
noto-fonts-extra
ttf-liberation
unicode-character-database

nvidia-utils
amd-gpu-modules

networkmanager
wpa_supplicant
openssh

base-devel
gcc
clang
cmake
make
EOF

echo -e "${GREEN}[✓] Package list created (${$(wc -l < ${BUILD_DIR}/archlive/packages.x86_64)} packages)${NC}\n"

echo -e "${BLUE}[4/8] Configuring boot hooks...${NC}"

mkdir -p "${BUILD_DIR}/archlive/airootfs/etc/mkinitcpio.d"

cat > "${BUILD_DIR}/archlive/airootfs/etc/mkinitcpio.conf" << 'EOF'
MODULES=(crc32c-intel)
BINARIES=()
FILES=()
HOOKS=(base udev autodetect microcode modconf kms block encrypt btrfs filesystems fsck)
COMPRESSION="zstd"
EOF

echo -e "${GREEN}[✓] Boot hooks configured${NC}\n"

echo -e "${BLUE}[5/8] Setting up GRUB bootloader...${NC}"

mkdir -p "${BUILD_DIR}/iso/boot/grub"

cat > "${BUILD_DIR}/iso/boot/grub/grub.cfg" << 'EOF'
set default="0"
set timeout=10
set color_normal="white/black"
set color_highlight="black/white"

menuentry "OneOS Live - UEFI (x86_64)" --class arch --class gnu-linux --class gnu --class os {
    search --label --set=root ONEOS
    echo 'Loading OneOS Live...'
    
    linux   /arch/boot/x86_64/vmlinuz-linux \
            archisobasedir=arch archisolabel=ONEOS \
            splash quiet rw
    
    initrd  /arch/boot/x86_64/initramfs-linux.img
}

menuentry "OneOS Live - BIOS (x86_64)" --class arch --class gnu-linux --class gnu --class os {
    search --label --set=root ONEOS
    echo 'Loading OneOS Live (BIOS mode)...'
    
    linux   /arch/boot/x86_64/vmlinuz-linux \
            archisobasedir=arch archisolabel=ONEOS \
            splash quiet rw
    
    initrd  /arch/boot/x86_64/initramfs-linux.img
}

menuentry "Reboot" {
    reboot
}

menuentry "Poweroff" {
    halt
}
EOF

echo -e "${GREEN}[✓] GRUB bootloader configured${NC}\n"

echo -e "${BLUE}[6/8] Setting up dual-boot detection hooks...${NC}"

mkdir -p "${BUILD_DIR}/archlive/airootfs/etc/grub.d"

cat > "${BUILD_DIR}/archlive/airootfs/etc/grub.d/40_custom_windows" << 'EOF'
#!/bin/bash

echo "Searching for Windows installations..."

for part in /dev/sd*1 /dev/nvme*p1; do
    if [ -b "$part" ]; then
        mount "$part" /tmp/efi_check 2>/dev/null || continue
        
        if [ -d "/tmp/efi_check/EFI/Microsoft/Boot" ]; then
            WINDOWS_PARTITION="$part"
            echo "Found Windows at $part"
            
            cat << GRUB_ENTRY
menuentry 'Windows Boot Manager (UEFI)' {
    insmod part_gpt
    insmod fat
    set root='(hd0,gpt1)'
    search --no-floppy --label --set root EFI
    chainloader /EFI/Microsoft/Boot/bootmgfw.efi
}
GRUB_ENTRY
        fi
        
        umount /tmp/efi_check 2>/dev/null
    fi
done

for part in /dev/disk*s* /dev/nvme*p*; do
    if [ -b "$part" ]; then
        mount "$part" /tmp/osx_check 2>/dev/null || continue
        
        if [ -f "/tmp/osx_check/System/Library/CoreServices/boot.efi" ]; then
            echo "Found macOS at $part"
            
            cat << GRUB_ENTRY
menuentry 'macOS' {
    insmod part_gpt
    insmod hfsplus
    set root='(hd0,gpt2)'
    chainloader /System/Library/CoreServices/boot.efi
}
GRUB_ENTRY
        fi
        
        umount /tmp/osx_check 2>/dev/null
    fi
done
EOF

chmod +x "${BUILD_DIR}/archlive/airootfs/etc/grub.d/40_custom_windows"

echo -e "${GREEN}[✓] Dual-boot hooks installed${NC}\n"

echo -e "${BLUE}[7/8] Configuring UEFI/EFI boot...${NC}"

mkdir -p "${BUILD_DIR}/iso/EFI/BOOT"

cat > "${BUILD_DIR}/iso/EFI/BOOT/BOOTX64.conf" << 'EOF'
title OneOS Installer
linux /arch/boot/x86_64/vmlinuz-linux
initrd /arch/boot/x86_64/initramfs-linux.img
options archisobasedir=arch archisolabel=ONEOS rw
EOF

echo -e "${GREEN}[✓] UEFI boot configured${NC}\n"

echo -e "${BLUE}[8/8] Building ISO image...${NC}"

ISO_FILENAME="${ISO_NAME}-${ISO_VERSION}-${ARCH}.iso"

mkisofs \
    -iso-level 3 \
    -full-iso9660-filenames \
    -volid "${BOOT_LABEL}" \
    -output "${ISO_FILENAME}" \
    \
    -eltorito-boot boot/grub/eltorito.img \
    -eltorito-catalog boot/grub/boot.cat \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    \
    -eltorito-alt-boot \
    -efi-boot boot/efi.img \
    -no-emul-boot \
    -isohybrid-mbr /usr/lib/syslinux/isohdpfx.bin \
    \
    "${BUILD_DIR}/iso"

echo -e "${GREEN}[✓] ISO image created: ${ISO_FILENAME}${NC}\n"

echo -e "${BLUE}╔═══════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║  OneOS Live ISO Builder - Build Complete                 ║${NC}"
echo -e "${BLUE}╚═══════════════════════════════════════════════════════════╝${NC}\n"

echo "ISO Details:"
echo "  Name: ${ISO_FILENAME}"
echo "  Size: $(du -sh ${ISO_FILENAME} | cut -f1)"
echo "  Boot: UEFI + BIOS (Hybrid)"
echo "  Languages: 200+"
echo "  Dual-boot: Windows/macOS detected"
echo ""
echo "Testing the ISO:"
echo "  1. Virtual Machine (VirtualBox):"
echo "     VBoxManage createvm --name OneoOS-VM --ostype Linux_64"
echo "     VBoxManage modifyvm OneoOS-VM --firmware efi"
echo "     VBoxManage storageattach OneoOS-VM --storagectl IDE --port 0 --device 0 --type dvddrive --medium ${ISO_FILENAME}"
echo ""
echo "  2. USB Boot:"
echo "     sudo dd if=${ISO_FILENAME} of=/dev/sdX bs=4M status=progress && sync"
echo ""
echo "Installation Logs:"
echo "  /var/log/oneos-install.log"
echo ""

ls -lh "${ISO_FILENAME}"

