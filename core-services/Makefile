CORE_SERVICES_DIR := .

CC := gcc
CFLAGS := -Wall -Wextra -O2 -fPIC -I../include
LDFLAGS := -L../build/output

SRCS := daemon_manager.c service_manager.c process_manager.c logging_service.c \
        ipc_service.c event_service.c timer_service.c scheduler.c
OBJS := $(SRCS:.c=.o)
TARGET := core_services.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking core services..."
	ld -r -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
