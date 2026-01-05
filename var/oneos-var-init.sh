#!/bin/bash

set -e

VAR_BASE="/var"
BIN_DIR="/bin"
LIB_DIR="/lib"

echo "[*] OneOS /var System Initialization"
echo ""

echo "[*] Creating /var directory structure..."
mkdir -p $VAR_BASE/cache/{apt/archives/partial,fontconfig,man,package-manager}
mkdir -p $VAR_BASE/log/{applications,services,audit}
mkdir -p $VAR_BASE/run/{pid,lock,sockets}
mkdir -p $VAR_BASE/lib/{apt/lists/partial,apt/cache,dpkg/{info,updates,alternatives},rpm/signingkeys,systemd}
mkdir -p $VAR_BASE/spool/{cron,mail,print,at}
mkdir -p $VAR_BASE/tmp
mkdir -p $VAR_BASE/backups
mkdir -p $VAR_BASE/crash

echo "[*] Setting directory permissions..."
chmod 755 $VAR_BASE
chmod 755 $VAR_BASE/cache
chmod 755 $VAR_BASE/log
chmod 755 $VAR_BASE/run
chmod 755 $VAR_BASE/lib
chmod 755 $VAR_BASE/spool
chmod 777 $VAR_BASE/tmp
chmod 755 $VAR_BASE/backups
chmod 755 $VAR_BASE/crash

echo "[*] Initializing log files..."
touch $VAR_BASE/log/syslog
touch $VAR_BASE/log/auth.log
touch $VAR_BASE/log/kernel.log
touch $VAR_BASE/log/messages
touch $VAR_BASE/log/secure
touch $VAR_BASE/log/dmesg

chmod 640 $VAR_BASE/log/*

echo "[*] Initializing DPKG database..."
touch $VAR_BASE/lib/dpkg/status
touch $VAR_BASE/lib/dpkg/status.old
chmod 644 $VAR_BASE/lib/dpkg/status*

echo "[*] Initializing APT..."
touch $VAR_BASE/lib/apt/lists/sources.list

echo "[*] Installing service files..."
cp oneos-var.service /etc/systemd/system/ 2>/dev/null || true
cp oneos-var-cleanup.sh $BIN_DIR/ 2>/dev/null || true
chmod +x $BIN_DIR/oneos-var-cleanup.sh 2>/dev/null || true

echo "[*] Running initialization..."
if [ -f "$BIN_DIR/var_manager" ]; then
    $BIN_DIR/var_manager init
else
    echo "[!] var_manager not found at $BIN_DIR/var_manager"
fi

echo ""
echo "[+] Initialization complete!"
echo ""
echo "System ready. Use 'var_manager' to manage the /var system."

exit 0

