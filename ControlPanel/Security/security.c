#include "security.h"

int security_init(void)
{
    return 0;
}

int security_get_status(security_status_t *status)
{
    if (!status)
    {
        return -1;
    }
    return 0;
}

int security_enable_firewall(void)
{
    return 0;
}

int security_disable_firewall(void)
{
    return 0;
}

int security_enable_antivirus(void)
{
    return 0;
}

int security_disable_antivirus(void)
{
    return 0;
}

int security_run_scan(uint32_t scan_type)
{
    return 0;
}

int security_get_scan_progress(uint32_t *progress)
{
    if (!progress)
    {
        return -1;
    }
    return 0;
}

int security_quarantine_threat(const char *threat_path)
{
    if (!threat_path)
    {
        return -1;
    }
    return 0;
}

int security_get_stats(security_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
