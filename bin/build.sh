#!/bin/bash

BIN_DIR="${0%/*}"
CC=${CC:-gcc}
CFLAGS="-Wall -Wextra -O2 -std=c99"
OUTPUT_DIR="$BIN_DIR/compiled"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

mkdir -p "$OUTPUT_DIR"

echo -e "${YELLOW}=== OneOS Bin Utilities Build System ===${NC}"
echo "CC: $CC"
echo "CFLAGS: $CFLAGS"
echo ""

compile_utility() {
    local category=$1
    local name=$2
    local source="$BIN_DIR/$category/${name}.c"
    local output="$OUTPUT_DIR/${name}"
    
    if [ ! -f "$source" ]; then
        echo -e "${RED}✗${NC} $category/$name.c not found"
        return 1
    fi
    
    echo -n "Compiling $category/$name... "
    if $CC $CFLAGS -o "$output" "$source" 2>/dev/null; then
        chmod +x "$output"
        echo -e "${GREEN}✓${NC}"
        return 0
    else
        echo -e "${RED}✗${NC}"
        return 1
    fi
}

echo -e "${YELLOW}Core Utilities:${NC}"
compile_utility "core" "pwd"
compile_utility "core" "echo"
compile_utility "core" "printf"
compile_utility "core" "cd"

echo -e "${YELLOW}File Utilities:${NC}"
compile_utility "file-utilities" "ls"
compile_utility "file-utilities" "mkdir"
compile_utility "file-utilities" "rm"
compile_utility "file-utilities" "wc"

echo -e "${YELLOW}Text Processing:${NC}"
compile_utility "text-processing" "head"
compile_utility "text-processing" "tail"
compile_utility "text-processing" "grep"
compile_utility "text-processing" "sort"

echo -e "${YELLOW}System Utilities:${NC}"
compile_utility "system-utilities" "uname"
compile_utility "system-utilities" "date"
compile_utility "system-utilities" "whoami"

echo -e "${YELLOW}Build Tools:${NC}"
compile_utility "build-tools" "nm"
compile_utility "build-tools" "objdump"
compile_utility "build-tools" "ar"

echo -e "${YELLOW}Mount Utilities:${NC}"
compile_utility "mount-utilities" "mount"
compile_utility "mount-utilities" "umount"

echo ""
echo -e "${GREEN}Build complete!${NC}"
echo "Compiled utilities are in: $OUTPUT_DIR"
echo ""
echo "Usage:"
echo "  Install: sudo cp $OUTPUT_DIR/* /usr/local/bin/"
echo "  Or use in PATH: export PATH=$OUTPUT_DIR:\$PATH"

