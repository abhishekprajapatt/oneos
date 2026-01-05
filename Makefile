KERNEL_DIR := kernel
DRIVERS_DIR := drivers
BIN_DIR := bin
FS_DIR := fs
ARCH_DIR := arch
BUILD_DIR := build
OUTPUT_DIR := $(BUILD_DIR)/output

CC := gcc
CFLAGS := -Wall -Wextra -O2 -fPIC
LDFLAGS := -L$(OUTPUT_DIR)

KERNEL_OBJS := $(KERNEL_DIR)/*.o
DRIVER_OBJS := $(DRIVERS_DIR)/**/*.o
BIN_OBJS := $(BIN_DIR)/**/*.o
FS_OBJS := $(FS_DIR)/**/*.o

.PHONY: all clean build drivers kernel bin fs arch

all: build drivers kernel bin fs arch

kernel:
	@echo "Building kernel..."
	$(MAKE) -C $(KERNEL_DIR)

drivers:
	@echo "Building drivers..."
	$(MAKE) -C $(DRIVERS_DIR)

bin:
	@echo "Building bin utilities..."
	$(MAKE) -C $(BIN_DIR)

fs:
	@echo "Building filesystem..."
	$(MAKE) -C $(FS_DIR)

arch:
	@echo "Building architecture modules..."
	$(MAKE) -C $(ARCH_DIR)

build:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OUTPUT_DIR)

clean:
	$(MAKE) -C $(KERNEL_DIR) clean
	$(MAKE) -C $(DRIVERS_DIR) clean
	$(MAKE) -C $(BIN_DIR) clean
	$(MAKE) -C $(FS_DIR) clean
	$(MAKE) -C $(ARCH_DIR) clean
	rm -rf $(BUILD_DIR)

distclean: clean
	@echo "Cleaning all build artifacts..."

help:
	@echo "OnEOS Build System"
	@echo "Targets:"
	@echo "  all       - Build all components"
	@echo "  kernel    - Build kernel"
	@echo "  drivers   - Build drivers"
	@echo "  bin       - Build binary utilities"
	@echo "  fs        - Build filesystem"
	@echo "  arch      - Build architecture modules"
	@echo "  clean     - Clean build artifacts"
	@echo "  distclean - Clean everything"
