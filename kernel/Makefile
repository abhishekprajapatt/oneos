KERNEL_DIR := .

CC := gcc
CFLAGS := -Wall -Wextra -O2 -fPIC -I../include
LDFLAGS := -L../build/output

KERNEL_SRCS := $(wildcard *.c)
KERNEL_OBJS := $(KERNEL_SRCS:.c=.o)
KERNEL_TARGET := kernel.o

.PHONY: all clean

all: $(KERNEL_TARGET)

$(KERNEL_TARGET): $(KERNEL_OBJS)
	@echo "Linking kernel..."
	ld -r -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(KERNEL_OBJS) $(KERNEL_TARGET)

.PHONY: help
help:
	@echo "Kernel Build"
	@echo "Targets: all, clean"
