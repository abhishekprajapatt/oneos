#!/bin/bash

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

ARCH=${ARCH:-x86_64}
CROSS_COMPILE=${CROSS_COMPILE:-}
BUILD_DIR=${BUILD_DIR:-./build}
KERNEL_DIR="kernel"
DRIVERS_DIR="drivers"
APPS_DIR="Applications"
LIB_DIR="lib"
OUTPUT_DIR="${BUILD_DIR}/output"
KERNEL_NAME="oneos-kernel"
KERNEL_ELF="${KERNEL_NAME}.elf"
KERNEL_BIN="${KERNEL_NAME}.bin"
LINKER_SCRIPT="kernel.ld"

print_header() {
    echo -e "${BLUE}================================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}================================================${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}→ $1${NC}"
}

check_prerequisites() {
    print_info "Checking prerequisites..."
    
    command -v gcc >/dev/null 2>&1 || { print_error "gcc not found"; exit 1; }
    command -v make >/dev/null 2>&1 || { print_error "make not found"; exit 1; }
    command -v ld >/dev/null 2>&1 || { print_error "ld not found"; exit 1; }
    
    print_success "All prerequisites met"
}

setup_directories() {
    print_info "Setting up build directories..."
    mkdir -p "${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}/obj"
    mkdir -p "${BUILD_DIR}/bin"
    mkdir -p "${OUTPUT_DIR}"
    print_success "Build directories created"
}

build_kernel() {
    print_header "Building Kernel"
    
    if [ ! -d "$KERNEL_DIR" ]; then
        print_error "Kernel directory not found: $KERNEL_DIR"
        return 1
    fi
    
    cd "$KERNEL_DIR"
    make ARCH="$ARCH" CROSS_COMPILE="$CROSS_COMPILE" || {
        print_error "Kernel build failed"
        cd ..
        return 1
    }
    cd ..
    
    cp "${KERNEL_DIR}/vmlinuz" "${OUTPUT_DIR}/vmlinuz" 2>/dev/null || true
    print_success "Kernel build complete"
}

build_drivers() {
    print_header "Building Device Drivers"
    
    if [ ! -d "$DRIVERS_DIR" ]; then
        print_error "Drivers directory not found: $DRIVERS_DIR"
        return 1
    fi
    
    cd "$DRIVERS_DIR"
    make ARCH="$ARCH" CROSS_COMPILE="$CROSS_COMPILE" || {
        print_error "Drivers build failed"
        cd ..
        return 1
    }
    cd ..
    
    find "${DRIVERS_DIR}" -name "*.o" -o -name "*.a" | xargs -I {} cp {} "${OUTPUT_DIR}/" 2>/dev/null || true
    print_success "Drivers build complete"
}

build_libraries() {
    print_header "Building System Libraries"
    
    if [ ! -d "$LIB_DIR" ]; then
        print_info "Libraries directory not found, skipping"
        return 0
    fi
    
    cd "$LIB_DIR"
    make ARCH="$ARCH" CROSS_COMPILE="$CROSS_COMPILE" || {
        print_error "Libraries build failed"
        cd ..
        return 1
    }
    cd ..
    
    find "${LIB_DIR}" -name "*.so" -o -name "*.a" | xargs -I {} cp {} "${OUTPUT_DIR}/" 2>/dev/null || true
    print_success "Libraries build complete"
}

build_applications() {
    print_header "Building System Applications"
    
    if [ ! -d "$APPS_DIR" ]; then
        print_info "Applications directory not found, skipping"
        return 0
    fi
    
    cd "$APPS_DIR"
    make ARCH="$ARCH" CROSS_COMPILE="$CROSS_COMPILE" || {
        print_error "Applications build failed"
        cd ..
        return 1
    }
    cd ..
    
    find "${APPS_DIR}" -type f -executable | xargs -I {} cp {} "${OUTPUT_DIR}/" 2>/dev/null || true
    print_success "Applications build complete"
}

collect_object_files() {
    print_header "Collecting Object Files"
    
    O_FILES=$(find . -name "*.o" -type f 2>/dev/null | sort)
    O_COUNT=$(echo "$O_FILES" | wc -l)
    
    if [ "$O_COUNT" -eq 0 ]; then
        print_error "No object files found"
        return 1
    fi
    
    print_info "Found $O_COUNT object files"
    TOTAL_SIZE=0
    for obj in $O_FILES; do
        SIZE=$(stat -c%s "$obj" 2>/dev/null || stat -f%z "$obj" 2>/dev/null)
        TOTAL_SIZE=$((TOTAL_SIZE + SIZE))
    done
    
    TOTAL_SIZE_KB=$((TOTAL_SIZE / 1024))
    print_success "$TOTAL_SIZE_KB KB total"
    
    echo "$O_FILES"
}

link_kernel() {
    print_header "Linking Kernel Binary"
    
    if [ ! -f "$LINKER_SCRIPT" ]; then
        print_error "Linker script not found: $LINKER_SCRIPT"
        return 1
    fi
    
    print_info "Collecting object files..."
    O_FILES=$(collect_object_files)
    
    print_info "Linking with ld..."
    if ld -T "$LINKER_SCRIPT" -o "$KERNEL_ELF" $O_FILES -Map kernel.map 2>/dev/null; then
        print_success "Kernel ELF created: $KERNEL_ELF"
    else
        print_error "Linking failed"
        return 1
    fi
    
    print_info "Converting to binary format..."
    if objcopy -O binary "$KERNEL_ELF" "$KERNEL_BIN"; then
        ELF_SIZE=$(stat -c%s "$KERNEL_ELF" 2>/dev/null || stat -f%z "$KERNEL_ELF" 2>/dev/null)
        BIN_SIZE=$(stat -c%s "$KERNEL_BIN" 2>/dev/null || stat -f%z "$KERNEL_BIN" 2>/dev/null)
        print_success "Kernel BIN created: $KERNEL_BIN"
        print_info "ELF: $((ELF_SIZE / 1024)) KB | BIN: $((BIN_SIZE / 1024)) KB"
    else
        print_error "Binary conversion failed"
        return 1
    fi
}

create_iso() {
    print_header "Creating Bootable ISO"
    
    print_info "Setting up ISO directory structure..."
    mkdir -p iso-root/boot/grub
    
    print_info "Creating GRUB configuration..."
    cat > iso-root/boot/grub/grub.cfg << 'GRUBCFG'
menuentry 'OneOS - 40-Part Architecture' {
    multiboot /boot/oneos-kernel.bin
    boot
}
GRUBCFG
    
    print_info "Copying kernel binary..."
    cp "$KERNEL_BIN" iso-root/boot/
    
    print_info "Creating ISO image..."
    if grub-mkrescue -o oneos.iso iso-root 2>/dev/null; then
        ISO_SIZE=$(stat -c%s oneos.iso 2>/dev/null || stat -f%z oneos.iso 2>/dev/null)
        print_success "ISO created: oneos.iso ($((ISO_SIZE / 1024 / 1024)) MB)"
    else
        print_info "grub-mkrescue not available, attempting alternative method..."
        if mkisofs -o oneos.iso -b boot/oneos-kernel.bin iso-root 2>/dev/null; then
            ISO_SIZE=$(stat -c%s oneos.iso 2>/dev/null || stat -f%z oneos.iso 2>/dev/null)
            print_success "ISO created: oneos.iso"
        else
            print_error "ISO creation skipped (tools not available)"
        fi
    fi
}

compile_tests() {
    print_header "Compiling Tests"
    
    if [ ! -d "tests" ]; then
        print_info "Tests directory not found, skipping"
        return 0
    fi
    
    print_info "Compiling integration tests..."
    if gcc -O2 -Wall tests/integration_test.c -o tests/integration_test 2>/dev/null; then
        TEST_SIZE=$(stat -c%s tests/integration_test 2>/dev/null || stat -f%z tests/integration_test 2>/dev/null)
        print_success "integration_test ($((TEST_SIZE / 1024)) KB)"
    fi
    
    print_info "Compiling performance benchmarks..."
    if gcc -O2 -Wall tests/performance_bench.c -o tests/performance_bench 2>/dev/null; then
        BENCH_SIZE=$(stat -c%s tests/performance_bench 2>/dev/null || stat -f%z tests/performance_bench 2>/dev/null)
        print_success "performance_bench ($((BENCH_SIZE / 1024)) KB)"
    fi
}

show_summary() {
    print_header "Build Summary"
    
    echo -e "${CYAN}Configuration:${NC}"
    echo -e "  Architecture: ${BLUE}${ARCH}${NC}"
    echo -e "  Build directory: ${BLUE}${BUILD_DIR}${NC}"
    echo -e "  Output directory: ${BLUE}${OUTPUT_DIR}${NC}"
    echo ""
    
    echo -e "${CYAN}Artifacts:${NC}"
    if [ -f "$KERNEL_ELF" ]; then
        ELF_SIZE=$(stat -c%s "$KERNEL_ELF" 2>/dev/null || stat -f%z "$KERNEL_ELF" 2>/dev/null)
        echo -e "  • ${BLUE}${KERNEL_ELF}${NC} ($((ELF_SIZE / 1024)) KB)"
    fi
    
    if [ -f "$KERNEL_BIN" ]; then
        BIN_SIZE=$(stat -c%s "$KERNEL_BIN" 2>/dev/null || stat -f%z "$KERNEL_BIN" 2>/dev/null)
        echo -e "  • ${BLUE}${KERNEL_BIN}${NC} ($((BIN_SIZE / 1024)) KB)"
    fi
    
    if [ -f "oneos.iso" ]; then
        ISO_SIZE=$(stat -c%s oneos.iso 2>/dev/null || stat -f%z oneos.iso 2>/dev/null)
        echo -e "  • ${BLUE}oneos.iso${NC} ($((ISO_SIZE / 1024 / 1024)) MB)"
    fi
    
    echo -e "  • ${BLUE}kernel.map${NC} (Link map)"
    
    if [ -d "$OUTPUT_DIR" ]; then
        file_count=$(find "$OUTPUT_DIR" -type f 2>/dev/null | wc -l)
        echo -e "  • ${BLUE}${OUTPUT_DIR}${NC} ($file_count files)"
    fi
}

main() {
    print_header "OneOS 40-Part Architecture - Complete Build System"
    echo "Configuration: $ARCH | Output: $OUTPUT_DIR"
    echo ""
    
    check_prerequisites
    setup_directories
    
    # Compile all subsystems
    build_kernel || exit 1
    echo ""
    
    build_drivers || exit 1
    echo ""
    
    build_libraries || exit 1
    echo ""
    
    build_applications || exit 1
    echo ""
    
    # Link kernel
    link_kernel || exit 1
    echo ""
    
    # Create bootable ISO
    create_iso || true
    echo ""
    
    # Compile tests
    compile_tests || true
    echo ""
    
    show_summary
    echo ""
    print_success "OneOS build completed successfully!"
    echo -e "Ready for deployment: ${BLUE}oneos.iso${NC}"
}

trap "print_error 'Build interrupted'; exit 130" INT TERM

main "$@"

