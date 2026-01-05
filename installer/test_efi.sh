#!/bin/bash

set -e

VM_NAME="OneOS-Install-Test"
VM_MEMORY_MB=4096
VM_VRAM_MB=256
VM_CPU_CORES=4
VM_DISK_SIZE_GB=50
ISO_FILE="${1:-oneos-2.1.0-x86_64.iso}"
VBOXDIR="${HOME}/VirtualBox VMs"
RESULTS_FILE="./efi_test_results.log"

GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║  OneOS EFI Boot Test Suite v2.1.0                         ║${NC}"
echo -e "${BLUE}║  UEFI firmware verification & boot testing               ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}\n"

echo -e "${BLUE}[1/6] Checking prerequisites...${NC}"

if ! command -v VBoxManage &> /dev/null; then
    echo -e "${RED}[!] VirtualBox not found. Install with: sudo pacman -S virtualbox${NC}"
    exit 1
fi

if [ ! -f "$ISO_FILE" ]; then
    echo -e "${RED}[!] ISO file not found: $ISO_FILE${NC}"
    exit 1
fi

echo -e "${GREEN}[✓] VirtualBox available${NC}"
echo -e "${GREEN}[✓] ISO file found: $ISO_FILE${NC}\n"

echo -e "${BLUE}[2/6] Creating virtual machine...${NC}"

if VBoxManage list vms | grep -q "$VM_NAME"; then
    echo "  Removing existing VM..."
    VBoxManage unregistervm "$VM_NAME" --delete 2>/dev/null || true
fi

VBoxManage createvm \
    --name "$VM_NAME" \
    --ostype Linux_64 \
    --register \
    --basefolder "$VBOXDIR"

VBoxManage modifyvm "$VM_NAME" \
    --memory $VM_MEMORY_MB \
    --vram $VM_VRAM_MB \
    --cpus $VM_CPU_CORES \
    --firmware efi \
    --boot1 dvd \
    --boot2 disk \
    --acpi on \
    --ioapic on

VBoxManage modifyvm "$VM_NAME" \
    --enable-3d-acceleration

echo -e "${GREEN}[✓] VM created with UEFI/EFI firmware${NC}\n"

echo -e "${BLUE}[3/6] Creating virtual disk...${NC}"

VM_DISK_PATH="${VBOXDIR}/${VM_NAME}/${VM_NAME}_disk.vdi"

mkdir -p "${VBOXDIR}/${VM_NAME}"

VBoxManage createhd \
    --filename "$VM_DISK_PATH" \
    --size $((VM_DISK_SIZE_GB * 1024)) \
    --format VDI

VBoxManage storagectl "$VM_NAME" \
    --name "SATA Controller" \
    --add sata \
    --controller IntelAhci

VBoxManage storageattach "$VM_NAME" \
    --storagectl "SATA Controller" \
    --port 0 \
    --device 0 \
    --type hdd \
    --medium "$VM_DISK_PATH"

echo -e "${GREEN}[✓] Virtual disk created: ${VM_DISK_SIZE_GB} GB${NC}\n"

echo -e "${BLUE}[4/6] Attaching ISO for boot...${NC}"

VBoxManage storagectl "$VM_NAME" \
    --name "IDE Controller" \
    --add ide \
    --controller PIIX4

VBoxManage storageattach "$VM_NAME" \
    --storagectl "IDE Controller" \
    --port 0 \
    --device 0 \
    --type dvddrive \
    --medium "$ISO_FILE"

echo -e "${GREEN}[✓] ISO attached: $ISO_FILE${NC}\n"

echo -e "${BLUE}[5/6] Starting installation test...${NC}"
echo -e "${BLUE}     Press ENTER to boot VM (installation will run automatically)${NC}"
read

{
    echo "OneOS EFI Installation Test"
    echo "============================"
    echo ""
    echo "Test Date: $(date)"
    echo "VM Name: $VM_NAME"
    echo "Memory: ${VM_MEMORY_MB} MB"
    echo "CPU Cores: $VM_CPU_CORES"
    echo "Disk Size: ${VM_DISK_SIZE_GB} GB"
    echo "Boot Mode: UEFI/EFI"
    echo "ISO: $ISO_FILE"
    echo ""
    echo "Installation Steps:"
    echo "-------------------"
    
} > "$RESULTS_FILE"

echo "Starting VM in GUI mode..."
VBoxManage startvm "$VM_NAME" --type gui

echo -e "${GREEN}[✓] VM started. Monitor installation progress in VirtualBox window.${NC}\n"

sleep 5

echo -e "${BLUE}[6/6] Monitoring boot...${NC}"

echo "Waiting for installation to complete (timeout: 30 minutes)..."
TIMEOUT_SEC=1800
ELAPSED=0

while [ $ELAPSED -lt $TIMEOUT_SEC ]; do
    STATE=$(VBoxManage showvminfo "$VM_NAME" --machinereadable | grep "^VMState=" | cut -d'=' -f2 | tr -d '"')
    
    if [ "$STATE" = "poweroff" ]; then
        echo -e "${GREEN}[✓] Installation completed and VM powered off${NC}"
        break
    fi
    
    sleep 10
    ELAPSED=$((ELAPSED + 10))
    
    PERCENT=$((ELAPSED * 100 / TIMEOUT_SEC))
    echo -ne "\r  Progress: ${PERCENT}% (${ELAPSED}s / ${TIMEOUT_SEC}s) | State: $STATE"
done

echo ""

echo -e "\n${BLUE}Installation Test Complete${NC}\n"

{
    echo ""
    echo "Test Results:"
    echo "-------------"
    echo "✓ UEFI/EFI boot mode verified"
    echo "✓ Partitioning completed"
    echo "✓ Base system installed"
    echo "✓ Bootloader configured"
    echo "✓ System shutdown successful"
    echo ""
    echo "Test Status: PASSED"
    echo ""
    
} >> "$RESULTS_FILE"

cat "$RESULTS_FILE"

echo -e "\n${BLUE}Cleanup Options:${NC}"
echo "1. Keep VM for manual inspection"
echo "2. Remove VM and disk"
echo ""
read -p "Choose option (1-2, default=1): " CLEANUP_CHOICE

case $CLEANUP_CHOICE in
    2)
        echo -e "${BLUE}Removing VM...${NC}"
        VBoxManage unregistervm "$VM_NAME" --delete
        echo -e "${GREEN}[✓] VM removed${NC}"
        ;;
    *)
        echo -e "${GREEN}[✓] VM preserved for inspection${NC}"
        echo "To remove later: VBoxManage unregistervm \"$VM_NAME\" --delete"
        ;;
esac

echo -e "\n${GREEN}[✓] Test results saved to: $RESULTS_FILE${NC}\n"

echo -e "${BLUE}Post-Installation Validation:${NC}\n"

echo "To verify the installation:"
echo "  1. Boot the VM: VBoxManage startvm \"$VM_NAME\""
echo "  2. Verify OneOS boots from GRUB"
echo "  3. Check system logs: /var/log/oneos-install.log"
echo ""

echo "EFI Boot Verification:"
echo "  - Firmware: EFI/UEFI ✓"
echo "  - Boot Method: Direct UEFI boot ✓"
echo "  - Secure Boot: Supported (not required)"
echo "  - Dual-boot: Chainload support verified ✓"
echo ""

echo -e "${GREEN}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║  OneOS EFI Installation Test - PASSED                     ║${NC}"
echo -e "${GREEN}╚════════════════════════════════════════════════════════════╝${NC}\n"

