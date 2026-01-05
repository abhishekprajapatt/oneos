ARCH_DIR := .

CC := gcc
CFLAGS := -Wall -Wextra -O2 -fPIC -I../include
LDFLAGS := -L../build/output

SUBDIRS := x86 arm64 riscv

.PHONY: all clean $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	@echo "Building $@..."
	$(MAKE) -C $@

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
