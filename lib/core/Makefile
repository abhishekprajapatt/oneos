obj-y := lib_core.o

all:
	$(CC) -c lib_core.c -o lib_core.o

clean:
	rm -f lib_core.o

install:
	cp lib_core.h /usr/include/
	cp lib_core.o /usr/lib/

.PHONY: all clean install
