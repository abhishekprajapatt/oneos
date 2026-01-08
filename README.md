# ONEOS - Operating System

A complete operating system implementation with kernel, drivers, filesystem,
networking, security, and system services. Production-ready code built from scratch.

## Quick Start

- Build: bash build.sh
- Test: ./tests/integration_test
- Benchmark: ./tests/performance_bench

## Essential Information

All users should know:

- Requirements: GCC, GNU Make, LD, Objcopy
- Build Instructions: See Building section below
- Architecture: x86-64, ARM64, RISC-V
- License: GPL v3 - See LICENSE file

Documentation and code available in repository.

# Who Are You?

- Developer - Building and extending OneOS
- Researcher - Studying OS architecture
- Student - Learning kernel internals
- Contributor - Adding features and fixes
- Driver Developer - Implementing hardware support

# For Specific Users

## Developer

Start here:

- oneos.c - Main entry point
- kernel/ - Kernel implementation
- drivers/ - Device driver framework
- tests/ - Integration and performance tests
- Build: bash build.sh

## Researcher

Explore kernel design:

- kernel/ - Scheduling, memory, IPC
- mm/ - Virtual memory and paging
- net/ - TCP/IP stack
- security/ - Cryptographic implementations
- core-services/ - System architecture

## Student

Learn OS concepts:

- oneos.c - Start here for overview
- kernel/scheduler.c - Process scheduling
- mm/ - Memory management
- fs/ - Filesystem implementation
- tests/ - See usage examples

## Driver Developer

Write hardware support:

- drivers/base/ - Driver framework
- drivers/block/ - Block devices
- drivers/char/ - Character devices
- drivers/network/ - Network drivers
- See Makefile for integration

## Contributor

Improve OneOS:

- Fork repository
- Create feature branch
- Implement changes
- Add tests
- Submit pull request

# Building

Prerequisites:

    GCC compiler
    GNU Make
    GNU LD
    Objcopy (binutils)

Build OneOS:

    bash build.sh

Output:

    oneos-kernel.elf - Kernel with symbols
    oneos-kernel.bin - Bootable kernel
    oneos.iso - Bootable ISO
    kernel.map - Symbol map

# Testing

Integration tests:

    ./tests/integration_test

Validates: kernel, drivers, filesystem, networking, security

Performance benchmarks:

    ./tests/performance_bench

Measures: memory, CPU, context switching, syscalls

# What's Included

- Kernel - Scheduling, memory, interrupts
- Drivers - Block, char, network, audio, USB, PCI
- Filesystem - ext4, FAT32, caching
- Networking - TCP/IP, DNS, routing
- Security - AES, SHA-256, RSA, TPM
- Services - Daemons, logging, IPC
- Tests - Integration and performance

Code Statistics

    802 C source files
    816 header files
    16 assembly files
    ~50,000 lines of code
    81 compiled objects (452 KB)

Architecture Support

    x86-64 with high memory mapping
    ARM64 with exception levels
    RISC-V with privilege levels

# License and Contributing

GPL v3 License - See LICENSE for details.

Open source for education, research, and development.

**What GPL v3 means:**

- You can use, modify, and redistribute OneOS
- Any modified version must also be open source under GPL v3
- You cannot rename OneOS and claim it as your own closed-source project
- You must provide source code with any distribution
- Commercial use is allowed, but source must be available

**Contributing:**

- Bug fixes and patches welcome
- Performance improvements wanted
- New drivers appreciated
- Feature proposals via issues
- All contributions remain under GPL v3

GPL ensures OneOS remains free and open for the community.

# Communication

- GitHub: https://github.com/abhishekprajapatt/oneos
- Issues: Bug reports and feature requests
- Discussions: Questions and ideas

## Resources

- Build System: build.sh, Makefile
- Kernel Layout: kernel.ld
- Tests: tests/ directory
- Source: Full implementation included
