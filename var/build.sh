#!/bin/bash

echo "=== OneOS /var System Compiler ==="
echo ""

CC=${CC:-gcc}
CFLAGS="-Wall -Wextra -O2 -fPIC"
LDFLAGS="-lc"

BUILD_DIR="./build"
BIN_DIR="./bin"
LIB_DIR="./lib"

mkdir -p $BUILD_DIR $BIN_DIR $LIB_DIR

echo "[*] Compiling core var system..."
$CC $CFLAGS -c var_system.c -o $BUILD_DIR/var_system.o
echo "[+] var_system.c"

echo "[*] Compiling subsystem modules..."

$CC $CFLAGS -c log/log_system.c -o $BUILD_DIR/log_system.o
echo "[+] log_system.c"

$CC $CFLAGS -c cache/cache_system.c -o $BUILD_DIR/cache_system.o
echo "[+] cache_system.c"

$CC $CFLAGS -c run/runtime_system.c -o $BUILD_DIR/runtime_system.o
echo "[+] runtime_system.c"

$CC $CFLAGS -c lib/package_manager.c -o $BUILD_DIR/package_manager.o
echo "[+] package_manager.c"

$CC $CFLAGS -c lib/dpkg/dpkg_database.c -o $BUILD_DIR/dpkg_database.o
echo "[+] dpkg_database.c"

$CC $CFLAGS -c lib/rpm/rpm_database.c -o $BUILD_DIR/rpm_database.o
echo "[+] rpm_database.c"

$CC $CFLAGS -c backups/backup_system.c -o $BUILD_DIR/backup_system.o
echo "[+] backup_system.c"

$CC $CFLAGS -c crash/crash_system.c -o $BUILD_DIR/crash_system.o
echo "[+] crash_system.c"

$CC $CFLAGS -c spool/spool_system.c -o $BUILD_DIR/spool_system.o
echo "[+] spool_system.c"

$CC $CFLAGS -c tmp/temp_system.c -o $BUILD_DIR/temp_system.o
echo "[+] temp_system.c"

echo ""
echo "[*] Building shared library..."
$CC $LDFLAGS -shared -fPIC -o $LIB_DIR/libvar.so \
    $BUILD_DIR/*.o

if [ -f $LIB_DIR/libvar.so ]; then
    echo "[+] libvar.so created"
else
    echo "[!] Failed to create shared library"
    exit 1
fi

echo ""
echo "[*] Building executable..."
$CC $CFLAGS -o $BIN_DIR/var_manager var_main.c \
    $BUILD_DIR/*.o $LDFLAGS

if [ -f $BIN_DIR/var_manager ]; then
    echo "[+] var_manager executable created"
else
    echo "[!] Failed to create executable"
    exit 1
fi

echo ""
echo "[*] Building complete!"
echo "    Library: $LIB_DIR/libvar.so"
echo "    Binary:  $BIN_DIR/var_manager"
echo ""

echo "[*] Build artifacts:"
ls -lh $BUILD_DIR/*.o
echo ""
ls -lh $LIB_DIR/
echo ""
ls -lh $BIN_DIR/

exit 0

