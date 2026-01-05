COMPAT_DIR := .

CC := gcc
CFLAGS := -Wall -Wextra -O2 -fPIC -I../include
LDFLAGS := -L../build/output

SRCS := posix.c system_call.c api_wrapper.c emulation.c library_loader.c abi_layer.c
OBJS := $(SRCS:.c=.o)
TARGET := compatibility-layer.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking compatibility layer..."
	ld -r -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
