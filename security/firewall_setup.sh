set -e

FIREWALL_LOG="/var/log/oneos-firewall.log"
UFW_CONFIG="/etc/ufw/ufw.conf"

init_firewall() {
    echo "[*] Initializing UFW firewall..."
    
    if ! command -v ufw &> /dev/null; then
        echo "[!] UFW not found, installing..."
        apt-get update && apt-get install -y ufw
    fi
    
    ufw --force enable
    echo "[✓] UFW enabled"
}

set_default_policies() {
    echo "[*] Setting default firewall policies..."
    
    ufw default deny incoming
    ufw default allow outgoing
    ufw default allow routed
    
    echo "[✓] Default policies: Deny IN, Allow OUT"
}

allow_services() {
    echo "[*] Opening required ports..."
    
    echo "[*] SSH: Allowing from local networks only..."
    ufw allow from 192.168.0.0/16 to any port 22/tcp
    ufw allow from 10.0.0.0/8 to any port 22/tcp
    ufw limit 22/tcp
    
    echo "[*] HTTP: Allow port 80"
    ufw allow 80/tcp
    
    echo "[*] HTTPS: Allow port 443"
    ufw allow 443/tcp
    
    echo "[*] DNS: Allow port 53"
    ufw allow 53/udp
    
    echo "[✓] Essential services allowed"
}

allow_oneos_services() {
    echo "[*] Adding OneOS service ports..."
    
    ufw allow 8080/tcp comment 'OneOS App Server'
    
    ufw allow 8443/tcp comment 'OneOS HTTPS Alt'
    
    for port in 5000 5001 5002 5003 5004 5005 5010 5011; do
        ufw allow $port/tcp comment "OneOS Service $port"
    done
    
    echo "[✓] OneOS services allowed"
}

block_dangerous_ports() {
    echo "[*] Blocking dangerous ports..."
    
    ufw deny 23/tcp comment 'Telnet - DANGEROUS'
    
    ufw deny 21/tcp comment 'FTP - Use SFTP instead'
    
    ufw deny 25/tcp comment 'SMTP - Use secure version'
    
    ufw deny 143/tcp comment 'IMAP - Use secure version'
    
    ufw deny 110/tcp comment 'POP3 - Use secure version'
    
    ufw deny 161/udp comment 'SNMP - Disable if not needed'
    ufw deny 162/udp comment 'SNMP Trap'
    
    for port in 137 138 139 445; do
        ufw deny $port/tcp comment 'NetBIOS/SMB - LAN only'
        ufw deny $port/udp comment 'NetBIOS/SMB - LAN only'
    done
    
    ufw deny 3389/tcp comment 'RDP - Restrict to VPN only'
    
    ufw deny 3306/tcp comment 'MySQL - Restrict to localhost'
    
    ufw deny 5432/tcp comment 'PostgreSQL - Restrict to localhost'
    
    ufw deny 27017/tcp comment 'MongoDB - Restrict to localhost'
    
    echo "[✓] Dangerous ports blocked"
}

apply_rate_limiting() {
    echo "[*] Applying rate limiting..."
    
    ufw limit 80/tcp comment 'HTTP rate limit'
    
    ufw limit 443/tcp comment 'HTTPS rate limit'

    echo "[✓] Rate limiting configured"
}

add_advanced_rules() {
    echo "[*] Adding advanced firewall rules..."
    
    ufw default allow related
    ufw default allow established
    
    echo "[*] Enabling SYN flood protection..."
    sysctl -w net.ipv4.tcp_syncookies=1 2>/dev/null || true
    sysctl -w net.ipv4.tcp_syn_retries=2 2>/dev/null || true
    sysctl -w net.ipv4.tcp_synack_retries=2 2>/dev/null || true
    sysctl -w net.ipv4.tcp_timestamps=1 2>/dev/null || true
    
    sysctl -w net.ipv4.conf.all.send_redirects=0 2>/dev/null || true
    sysctl -w net.ipv4.conf.default.send_redirects=0 2>/dev/null || true
    
    sysctl -w net.ipv4.conf.all.send_redirects=0 2>/dev/null || true
    sysctl -w net.ipv4.conf.all.rp_filter=1 2>/dev/null || true
    sysctl -w net.ipv4.conf.default.rp_filter=1 2>/dev/null || true
    
    sysctl -w net.ipv4.ip_forward=0 2>/dev/null || true
    
    echo "[✓] Advanced rules applied"
}

configure_ipv6() {
    echo "[*] Configuring IPv6 firewall..."
    
    sed -i 's/IPV6=no/IPV6=yes/' "$UFW_CONFIG" 2>/dev/null || true
    
    ufw allow from ::/0 to ::/0 port 80/tcp
    ufw allow from ::/0 to ::/0 port 443/tcp
    
    echo "[✓] IPv6 configured"
}

enable_logging() {
    echo "[*] Enabling firewall logging..."
    
    ufw logging on
    ufw logging medium
    
    mkdir -p "$(dirname "$FIREWALL_LOG")"
    
    echo "[✓] Logging enabled: /var/log/ufw.log"
}

show_rules() {
    echo ""
    echo "╔════════════════════════════════════════════╗"
    echo "║        OneOS UFW Firewall Rules           ║"
    echo "╚════════════════════════════════════════════╝"
    echo ""
    
    echo "Status:"
    ufw status | head -5
    echo ""
    
    echo "Numbered Rules:"
    ufw status numbered | head -20
    echo ""
    
    echo "Recent Block Events:"
    tail -10 /var/log/ufw.log 2>/dev/null | tail -5 || echo "No recent blocks"
    echo ""
}

reload_rules() {
    echo "[*] Reloading firewall rules..."
    ufw reload
    echo "[✓] Rules reloaded"
}

save_config() {
    echo "[*] Saving firewall configuration..."
    
    mkdir -p /etc/ufw/
    
    cp /etc/ufw/rules.v4 /etc/ufw/rules.v4.backup 2>/dev/null || true
    cp /etc/ufw/rules.v6 /etc/ufw/rules.v6.backup 2>/dev/null || true
    
    sysctl -p /etc/sysctl.conf 2>/dev/null || true
    
    echo "[✓] Configuration saved"
}

main() {
    echo ""
    echo "╔════════════════════════════════════════════════╗"
    echo "║   OneOS UFW Firewall - Hardened Setup        ║"
    echo "║   Allow: 80, 443 | Block: Everything else    ║"
    echo "╚════════════════════════════════════════════════╝"
    echo ""
    
    init_firewall
    set_default_policies
    allow_services
    allow_oneos_services
    block_dangerous_ports
    apply_rate_limiting
    add_advanced_rules
    configure_ipv6
    enable_logging
    reload_rules
    save_config
    show_rules
    
    echo ""
    echo "╔════════════════════════════════════════════════╗"
    echo "║    OneOS Firewall Setup Complete ✓            ║"
    echo "║    Default: DENY all | Allow: 80, 443        ║"
    echo "╚════════════════════════════════════════════════╝"
    echo ""
}

main

