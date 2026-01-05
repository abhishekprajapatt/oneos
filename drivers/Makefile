DRIVER_DIR := .

CC := gcc
CFLAGS := -Wall -Wextra -O2 -fPIC -I../../include
LDFLAGS := -L../../build/output

SUBDIRS := base acpi ata audio block display network pci usb storage

.PHONY: all clean $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	@echo "Building $@..."
	$(MAKE) -C $@

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

.PHONY: help
help:
	@echo "Drivers Build"
	@echo "Building all driver subsystems"
