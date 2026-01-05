#ifndef SECURITY_H
#define SECURITY_H

#include <stdint.h>

typedef struct
{
    uint32_t firewall_enabled;
    uint32_t antivirus_enabled;
    uint32_t auto_updates_enabled;
    const char *firewall_status;
    const char *last_update_time;
} security_status_t;

typedef struct
{
    uint32_t total_threats;
    uint32_t threats_blocked;
    uint32_t quarantined_items;
    uint64_t last_scan_time;
} security_stats_t;

int security_init(void);
int security_get_status(security_status_t *status);
int security_enable_firewall(void);
int security_disable_firewall(void);
int security_enable_antivirus(void);
int security_disable_antivirus(void);
int security_run_scan(uint32_t scan_type);
int security_get_scan_progress(uint32_t *progress);
int security_quarantine_threat(const char *threat_path);
int security_get_stats(security_stats_t *stats);

#endif
