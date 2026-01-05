#!/bin/bash

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

BUILD_DIR="build"
INSTALL_DIR="/opt/oneos"
DRIVER_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PARALLEL_JOBS=$(nproc)
VERBOSE=0
DEBUG=0

print_header() {
    echo -e "${BLUE}════════════════════════════════════════════════════════════${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}════════════════════════════════════════════════════════════${NC}"
}

print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

usage() {
    cat << EOF
OneOS Driver Build System

Usage: $0 [OPTIONS] [TARGET]

OPTIONS:
    -h, --help              Show this help message
    -v, --verbose           Enable verbose output
    -d, --debug             Enable debug symbols
    -j, --jobs N            Parallel jobs (default: $(nproc))
    -b, --build-dir DIR     Build directory (default: build)
    -i, --install-dir DIR   Install directory (default: /opt/oneos)
    -c, --config            Run configuration menu
    -C, --clean             Clean build artifacts
    -t, --test              Run tests

TARGETS:
    all                     Build all drivers (default)
    modules                 Build as loadable modules
    install                 Install built drivers
    clean                   Clean build artifacts
    help                    Show detailed help
    pci                     Build PCI driver only
    usb                     Build USB driver only
    i2c                     Build I2C driver only
    spi                     Build SPI driver only
    char                    Build character devices
    gpio                    Build GPIO driver
    network                 Build network drivers
    doc                     Generate documentation

EXAMPLES:
    $0 -j 4

    $0 -d pci

    $0 -c all

    $0 install

EOF
    exit 0
}

show_detailed_help() {
    cat << EOF
OneOS Driver Build System - Detailed Help

OVERVIEW:
The OneOS driver subsystem provides a comprehensive set of device drivers
for the OneOS operating system, supporting multiple architectures including
x86, ARM, and RISC-V.

BUILD PROCESS:
1. Configuration: Define which drivers to build
2. Compilation: Compile selected drivers
3. Installation: Install drivers to system
4. Testing: Verify driver functionality

CONFIGURATION:
To configure which drivers to build, use:
    make menuconfig
    make defconfig

DRIVER CATEGORIES:
- Bus Drivers: PCI, USB, I2C, SPI, Platform
- Character Devices: /dev/null, /dev/zero, /dev/random
- Network: Ethernet, Wireless, Bluetooth
- Storage: SATA, NVMe, SCSI, MMC
- Control: GPIO, Power, Thermal, Clock
- Hardware: GPU, Input, Sound, Media

COMPILATION:
By default, drivers are compiled as loadable modules (.ko files).
They can also be compiled directly into the kernel.

Module compilation:
    make modules
    make modules_install

Kernel integration:
    Enable options in Kconfig and rebuild kernel

INSTALLATION:
Drivers are installed using standard kernel module installation:
    sudo make modules_install

This copies .ko files to:
    /lib/modules/\$(uname -r)/kernel/drivers/

LOADING DRIVERS:
Load driver module:
    sudo insmod driver_name.ko

List loaded modules:
    lsmod

Unload driver:
    sudo rmmod driver_name

View kernel messages:
    dmesg | tail -20

DEBUGGING:
Enable debug output:
    echo 1 > /sys/kernel/debug/debug_enable

View driver information:
    cat /proc/drivers/oneos/status

Memory debugging:
    cat /sys/kernel/debug/kmemleak

PERFORMANCE PROFILING:
Profile driver performance:
    perf record -g driver_operation
    perf report

TROUBLESHOOTING:
If driver doesn't load:
1. Check kernel messages: dmesg
2. Verify driver exists: find . -name "*.ko"
3. Check compatibility: uname -a
4. Try with debug symbols: make DEBUG=1
5. Check dependencies: modinfo driver.ko

For more information, see:
- DRIVERS.md - API documentation
- DEVELOPMENT.md - Driver development guide
- README.md - Project overview

EOF
    exit 0
}

setup_build_dir() {
    if [ ! -d "$BUILD_DIR" ]; then
        print_info "Creating build directory: $BUILD_DIR"
        mkdir -p "$BUILD_DIR"
    fi
}

show_config() {
    print_header "Current Configuration"
    echo "Driver Directory: $DRIVER_DIR"
    echo "Build Directory:  $BUILD_DIR"
    echo "Install Directory: $INSTALL_DIR"
    echo "Parallel Jobs:    $PARALLEL_JOBS"
    echo "Verbose:          $VERBOSE"
    echo "Debug:            $DEBUG"
    echo "Kernel Version:   $(uname -r)"
    echo "Architecture:     $(uname -m)"
}

build_all() {
    print_header "Building All OneOS Drivers"
    
    show_config
    
    print_info "Building drivers..."
    cd "$DRIVER_DIR"
    
    if [ "$DEBUG" -eq 1 ]; then
        make -j "$PARALLEL_JOBS" DEBUG=1 V="$VERBOSE"
    else
        make -j "$PARALLEL_JOBS" V="$VERBOSE"
    fi
    
    print_info "Build complete!"
}

build_modules() {
    print_header "Building OneOS Driver Modules"
    
    cd "$DRIVER_DIR"
    print_info "Building kernel modules..."
    
    if [ "$DEBUG" -eq 1 ]; then
        make -j "$PARALLEL_JOBS" modules DEBUG=1 V="$VERBOSE"
    else
        make -j "$PARALLEL_JOBS" modules V="$VERBOSE"
    fi
    
    print_info "Module build complete!"
    echo "To install modules, run: sudo make modules_install"
}

build_specific() {
    local target=$1
    print_header "Building OneOS Driver: $target"
    
    cd "$DRIVER_DIR"
    
    case "$target" in
        pci)
            print_info "Building PCI driver..."
            make -j "$PARALLEL_JOBS" -C drivers/bus/pci V="$VERBOSE"
            ;;
        usb)
            print_info "Building USB driver..."
            make -j "$PARALLEL_JOBS" -C drivers/bus/usb V="$VERBOSE"
            ;;
        i2c)
            print_info "Building I2C driver..."
            make -j "$PARALLEL_JOBS" -C drivers/bus/i2c V="$VERBOSE"
            ;;
        spi)
            print_info "Building SPI driver..."
            make -j "$PARALLEL_JOBS" -C drivers/bus/spi V="$VERBOSE"
            ;;
        char)
            print_info "Building character device drivers..."
            make -j "$PARALLEL_JOBS" -C drivers/char V="$VERBOSE"
            ;;
        gpio)
            print_info "Building GPIO driver..."
            make -j "$PARALLEL_JOBS" -C drivers/gpio V="$VERBOSE"
            ;;
        network)
            print_info "Building network drivers..."
            make -j "$PARALLEL_JOBS" -C drivers/network V="$VERBOSE"
            ;;
        *)
            print_error "Unknown target: $target"
            echo "Available targets: pci, usb, i2c, spi, char, gpio, network"
            exit 1
            ;;
    esac
    
    print_info "Build complete!"
}

install_drivers() {
    print_header "Installing OneOS Drivers"
    
    if [ ! -w "$INSTALL_DIR" ]; then
        print_warn "Install directory $INSTALL_DIR not writable, using sudo"
        cd "$DRIVER_DIR"
        sudo make modules_install INSTALL_MOD_PATH="$INSTALL_DIR"
    else
        cd "$DRIVER_DIR"
        make modules_install INSTALL_MOD_PATH="$INSTALL_DIR"
    fi
    
    print_info "Installation complete!"
}

clean_build() {
    print_header "Cleaning Build Artifacts"
    
    cd "$DRIVER_DIR"
    print_info "Removing build artifacts..."
    
    make clean
    
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
    
    print_info "Clean complete!"
}

run_tests() {
    print_header "Running OneOS Driver Tests"
    
    cd "$DRIVER_DIR"
    
    if [ -f "tests/run_tests.sh" ]; then
        print_info "Running test suite..."
        bash tests/run_tests.sh
    else
        print_warn "Test suite not found at tests/run_tests.sh"
    fi
}

generate_docs() {
    print_header "Generating OneOS Driver Documentation"
    
    print_info "Documentation already available:"
    echo "  - DRIVERS.md (API Reference)"
    echo "  - DEVELOPMENT.md (Developer Guide)"
    echo "  - README.md (Project Overview)"
    echo "  - COMPLETION_SUMMARY.md (Implementation Status)"
}

configure_drivers() {
    print_header "Configuring OneOS Drivers"
    
    cd "$DRIVER_DIR"
    print_info "Running configuration menu..."
    
    if command -v menuconfig &> /dev/null; then
        make menuconfig
    else
        print_error "menuconfig not available"
        echo "To configure drivers, edit Kconfig manually or use:"
        echo "  make defconfig
        exit 1
    fi
}

main() {
    while [[ $
        case $1 in
            -h|--help)
                usage
                ;;
            --detailed-help)
                show_detailed_help
                ;;
            -v|--verbose)
                VERBOSE=1
                shift
                ;;
            -d|--debug)
                DEBUG=1
                shift
                ;;
            -j|--jobs)
                PARALLEL_JOBS=$2
                shift 2
                ;;
            -b|--build-dir)
                BUILD_DIR=$2
                shift 2
                ;;
            -i|--install-dir)
                INSTALL_DIR=$2
                shift 2
                ;;
            -c|--config)
                configure_drivers
                shift
                ;;
            -C|--clean)
                clean_build
                shift
                ;;
            -t|--test)
                run_tests
                shift
                ;;
            all)
                build_all
                exit 0
                ;;
            modules)
                build_modules
                exit 0
                ;;
            install)
                install_drivers
                exit 0
                ;;
            clean)
                clean_build
                exit 0
                ;;
            help)
                show_detailed_help
                ;;
            doc)
                generate_docs
                exit 0
                ;;
            pci|usb|i2c|spi|char|gpio|network)
                build_specific "$1"
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                usage
                ;;
        esac
    done
    
    build_all
}

main "$@"

