#!/bin/bash

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

ARCH=${ARCH:-x86_64}
CROSS_COMPILE=${CROSS_COMPILE:-}
BUILD_DIR=${BUILD_DIR:-./build}
KERNEL_DIR="kernel"
DRIVERS_DIR="drivers"
APPS_DIR="Applications"
LIB_DIR="lib"
OUTPUT_DIR="${BUILD_DIR}/output"

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

show_summary() {
    print_header "Build Summary"
    
    echo -e "Architecture: ${BLUE}${ARCH}${NC}"
    echo -e "Build directory: ${BLUE}${BUILD_DIR}${NC}"
    echo -e "Output directory: ${BLUE}${OUTPUT_DIR}${NC}"
    
    if [ -d "$OUTPUT_DIR" ]; then
        file_count=$(find "$OUTPUT_DIR" -type f | wc -l)
        echo -e "Built files: ${BLUE}${file_count}${NC}"
    fi
}

main() {
    print_header "OneOS Build System"
    echo "Starting build process..."
    echo ""
    
    check_prerequisites
    setup_directories
    
    build_kernel || exit 1
    echo ""
    
    build_drivers || exit 1
    echo ""
    
    build_libraries || exit 1
    echo ""
    
    build_applications || exit 1
    echo ""
    
    show_summary
    echo ""
    print_success "OneOS build completed successfully!"
    echo -e "Output is located in: ${BLUE}${OUTPUT_DIR}${NC}"
}

trap "print_error 'Build interrupted'; exit 130" INT TERM

main "$@"

