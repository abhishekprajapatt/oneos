# OneOS CLI System Makefile
# Production-Grade Command-Line Interface Build System

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O2 -fPIC -g
LDFLAGS = -lc

TARGET = oneos-cli
LIBNAME = liboneos-cli.a

SOURCES = cli.c commands.c extended_commands.c shell.c
HEADERS = cli.h

OBJECTS = $(SOURCES:.c=.o)
LIB_OBJECTS = cli.o commands.o extended_commands.c

.PHONY: all clean install build

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✓ Built $(TARGET)"

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "  Compiled $(SOURCES)"

lib: $(LIB_OBJECTS)
	ar rcs $(LIBNAME) $(LIB_OBJECTS)
	@echo "✓ Built $(LIBNAME)"

install: $(TARGET)
	@mkdir -p /usr/local/bin
	@cp $(TARGET) /usr/local/bin/
	@chmod +x /usr/local/bin/$(TARGET)
	@echo "✓ Installed $(TARGET) to /usr/local/bin"

clean:
	@rm -f *.o $(TARGET) $(LIBNAME)
	@echo "✓ Cleaned build artifacts"

help:
	@echo "OneOS CLI Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make all       - Build CLI system (default)"
	@echo "  make lib       - Build CLI library"
	@echo "  make install   - Install CLI to /usr/local/bin"
	@echo "  make clean     - Remove build artifacts"
	@echo "  make help      - Show this message"
