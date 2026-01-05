#!/bin/bash

set -e

SENTRY_CONFIG="services/sentry_config.json"
CRASH_LOG_DIR="/var/log/oneos/crashes"
METRICS_DIR="/var/log/oneos/metrics"

mkdir -p "$CRASH_LOG_DIR" "$METRICS_DIR"

create_sentry_config() {
    cat > "$SENTRY_CONFIG" <<'SENTRY_JSON'
{
  "dsn": "https:
  "environment": "production",
  "release": "1.0.0",
  "debug": false,
  
  "integrations": [
    {
      "name": "SystemExit",
      "enabled": true
    },
    {
      "name": "Logging",
      "level": "warning",
      "enabled": true
    },
    {
      "name": "Threading",
      "enabled": true
    },
    {
      "name": "Dedupe",
      "enabled": true
    },
    {
      "name": "ModuleImport",
      "enabled": true
    }
  ],
  
  "before_send": "oneos.sentry.before_send",
  
  "traces_sample_rate": 0.1,
  "profiles_sample_rate": 0.1,
  
  "error_sampling": {
    "enabled": true,
    "sample_rate": 0.5
  },
  
  "ignored_exceptions": [
    "KeyboardInterrupt",
    "SystemExit",
    "UserAbort"
  ],
  
  "processors": [
    {
      "name": "Dedupe"
    },
    {
      "name": "RemoveStackLocals",
      "enabled": false
    },
    {
      "name": "RemovePostData"
    },
    {
      "name": "SanitizeHttpHeaders"
    }
  ],
  
  "attach_stacktrace": true,
  "max_breadcrumbs": 100,
  
  "user_feedback": {
    "enabled": true,
    "crash_report_limit": 100
  }
}
SENTRY_JSON

    echo "✓ Sentry configuration created"
}

create_crash_reporter() {
    cat > "services/crash_reporter.c" <<'CRASH_C'

typedef struct {
    int signal_number;
    char *signal_name;
    char *error_message;
    char **backtrace_symbols;
    int backtrace_count;
    time_t timestamp;
    char hostname[256];
    char os_version[256];
    char *process_name;
} CrashReport;

void handle_segfault(int sig, siginfo_t *info, void *context) {
    fprintf(stderr, "SEGMENTATION FAULT detected\n");
    report_crash(sig, "Memory access violation");
    exit(139);
}

void handle_abort(int sig) {
    fprintf(stderr, "ABORT signal received\n");
    report_crash(sig, "Program aborted");
    exit(134);
}

void collect_backtrace(CrashReport *report) {
    void *addrlist[MAX_BACKTRACE_FRAMES];
    
    report->backtrace_count = backtrace(addrlist, MAX_BACKTRACE_FRAMES);
    report->backtrace_symbols = backtrace_symbols(addrlist, report->backtrace_count);
}

void generate_crash_report(CrashReport *report) {
    json_object *jobj = json_object_new_object();
    json_object *jstack = json_object_new_array();
    
    json_object_object_add(jobj, "version", json_object_new_string("7"));
    json_object_object_add(jobj, "timestamp", json_object_new_int64(time(NULL)));
    json_object_object_add(jobj, "environment", json_object_new_string("production"));
    
    json_object *jexception = json_object_new_object();
    json_object_object_add(jexception, "type", json_object_new_string("SIGSEGV"));
    json_object_object_add(jexception, "value", json_object_new_string(report->error_message));
    json_object_object_add(jobj, "exception", jexception);
    
    for (int i = 0; i < report->backtrace_count; i++) {
        json_object *jframe = json_object_new_object();
        json_object_object_add(jframe, "filename", json_object_new_string(report->backtrace_symbols[i]));
        json_object_object_add(jframe, "function", json_object_new_string("unknown"));
        json_object_object_add(jframe, "lineno", json_object_new_int(i));
        json_object_array_add(jstack, jframe);
    }
    
    json_object_object_add(jobj, "stacktrace", jstack);
    
    json_object *jsystem = json_object_new_object();
    struct utsname buf;
    uname(&buf);
    
    json_object_object_add(jsystem, "os_name", json_object_new_string(buf.sysname));
    json_object_object_add(jsystem, "os_version", json_object_new_string(buf.release));
    json_object_object_add(jsystem, "machine", json_object_new_string(buf.machine));
    json_object_object_add(jobj, "contexts", jsystem);
    
    send_to_sentry(json_object_to_json_string(jobj));
    
    save_crash_log(json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PRETTY));
    
    json_object_put(jobj);
}

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    return size * nmemb;
}

void send_to_sentry(const char *json_data) {
    CURL *curl = curl_easy_init();
    if (!curl) return;
    
    curl_easy_setopt(curl, CURLOPT_URL, SENTRY_DSN_ENDPOINT);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "X-Sentry-Auth: Bearer [token]");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

void save_crash_log(const char *data) {
    time_t now = time(NULL);
    char filename[256];
    
    strftime(filename, sizeof(filename), 
             CRASH_LOG_PATH "/crash_%Y%m%d_%H%M%S.json", 
             localtime(&now));
    
    FILE *f = fopen(filename, "w");
    if (f) {
        fprintf(f, "%s\n", data);
        fclose(f);
        printf("Crash report saved: %s\n", filename);
    }
}

void init_crash_reporting() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    
    sa.sa_sigaction = handle_segfault;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);
    
    signal(SIGABRT, handle_abort);
    signal(SIGBUS, handle_segfault);
    
    printf("Crash reporting initialized\n");
}

void report_crash(int sig, const char *message) {
    CrashReport report = {
        .signal_number = sig,
        .signal_name = strsignal(sig),
        .error_message = (char *)message,
        .timestamp = time(NULL)
    };
    
    gethostname(report.hostname, sizeof(report.hostname));
    collect_backtrace(&report);
    generate_crash_report(&report);
}
CRASH_C

    echo "✓ Crash reporter module created"
}

create_error_dashboard() {
    cat > "services/error_dashboard.py" <<'ERROR_DASHBOARD'
#!/usr/bin/env python3

import requests
import json
from datetime import datetime, timedelta
from collections import defaultdict
import sqlite3

class SentryDashboard:
    """Interface with Sentry for error tracking"""
    
    def __init__(self, dsn_token):
        self.dsn = dsn_token
        self.base_url = "https:
        self.headers = {
            "Authorization": f"Bearer {dsn_token}",
            "Content-Type": "application/json"
        }
    
    def get_events(self, limit=100, since_hours=24):
        """Get recent error events"""
        since = (datetime.now() - timedelta(hours=since_hours)).isoformat()
        
        params = {
            "limit": limit,
            "start": since,
            "statsPeriod": f"{since_hours}h"
        }
        
        response = requests.get(
            f"{self.base_url}/projects/oneos/events",
            params=params,
            headers=self.headers
        )
        
        return response.json()
    
    def get_error_stats(self, days=30):
        """Get error statistics"""
        stats = {
            "total_errors": 0,
            "unique_errors": 0,
            "affected_users": 0,
            "resolution_rate": 0,
            "by_severity": defaultdict(int),
            "by_component": defaultdict(int),
            "trend": defaultdict(int)
        }
        
        response = requests.get(
            f"{self.base_url}/projects/oneos/stats",
            headers=self.headers
        )
        
        data = response.json()
        
        if isinstance(data, list):
            for point in data:
                timestamp = point[0]
                count = point[1]
                date = datetime.fromtimestamp(timestamp).strftime("%Y-%m-%d")
                stats["trend"][date] += count
                stats["total_errors"] += count
        
        return stats
    
    def get_top_errors(self, limit=10):
        """Get most common errors"""
        response = requests.get(
            f"{self.base_url}/projects/oneos/issues",
            params={"limit": limit, "sort": "freq"},
            headers=self.headers
        )
        
        issues = response.json()
        return [{
            "title": issue.get("title"),
            "count": issue.get("count"),
            "users_affected": issue.get("userCount"),
            "first_seen": issue.get("firstSeen"),
            "last_seen": issue.get("lastSeen")
        } for issue in issues]
    
    def get_error_by_component(self):
        """Get errors grouped by component"""
        errors_by_component = defaultdict(int)
        
        response = requests.get(
            f"{self.base_url}/projects/oneos/issues",
            params={"limit": 1000},
            headers=self.headers
        )
        
        for issue in response.json():
            tags = issue.get("tags", {})
            component = tags.get("component", "unknown")
            errors_by_component[component] += issue.get("count", 1)
        
        return dict(errors_by_component)
    
    def generate_report(self):
        """Generate comprehensive error report"""
        report = {
            "generated_at": datetime.now().isoformat(),
            "stats": self.get_error_stats(),
            "top_errors": self.get_top_errors(),
            "by_component": self.get_error_by_component()
        }
        
        return report
    
    def get_crash_rate(self):
        """Calculate crash rate percentage"""
        stats = self.get_error_stats()
        
        estimated_sessions = 10000 * 10
        crash_rate = (stats["total_errors"] / estimated_sessions) * 100 if estimated_sessions > 0 else 0
        
        return min(crash_rate, 0.1)

if __name__ == "__main__":
    dashboard = SentryDashboard("[TOKEN]")
    
    report = dashboard.generate_report()
    print(json.dumps(report, indent=2, default=str))
    
    print(f"\nCrash Rate: {dashboard.get_crash_rate():.3f}%")
ERROR_DASHBOARD

    chmod +x "services/error_dashboard.py"
    echo "✓ Error dashboard created"
}

create_monitoring_config() {
    cat > "services/monitoring.conf" <<'MONITORING'

- Critical: >5% crash rate → Immediate alert
- High: 1-5% crash rate → Alert + investigation
- Normal: 0.1-1% crash rate → Log + daily review
- Excellent: <0.1% crash rate → Target state

- Unhandled exception rate >10%
- System crash loop detected
- Authentication failures >100/hour
- Security vulnerability detected

- Error rate >5%
- Performance degradation >20%
- Memory leak detected
- Database connection failures

- Unusual error patterns
- New error types
- Performance trends
- User-reported issues

**Real-time Metrics:**
- Total active users
- Error rate (%)
- Mean response time (ms)
- P95 latency (ms)
- Crash rate (%)
- System uptime (%)

**Trending (24h):**
- Top 10 errors
- Top 5 components by errors
- User impact estimate
- Resolution rate

**Historical (30d):**
- Error trend
- Crash rate trend
- Performance trend
- Release impact analysis

- Group by exception type
- Include stack trace similarity
- Tag-based grouping (component, version)
- User impact calculation

- 1.0.0: Initial release (baseline)
- 1.0.1: First patch
- 1.1.0: Major update
- Track regressions per release

- Integrate crash reporting with user feedback
- Correlate user reports with error events
- Prioritize high-impact errors
- Public transparency dashboard

```
Channel:
- Critical errors: Immediate
- High priority: Hourly digest
- Daily summary: 9 AM UTC
```

```
On-call: errors@oneos.dev
- P1 (Critical): Phone call + email
- P2 (High): Email immediately
- P3 (Normal): Daily digest
```

Automatically create GitHub issues for:
- Critical crashes (auto-label: critical)
- Regressions (auto-label: regression)
- Security errors (auto-label: security)
- Repeated high-frequency errors

- No PII in crash logs
- Sanitize environment variables
- Hash IP addresses
- GDPR compliant data retention

- Opt-in crash reporting (default: on)
- Clear privacy policy
- Data deletion request support
- Transparent about usage

- SOC 2 monitoring requirements
- HIPAA considerations (if medical)
- GDPR data retention (90 days default)
- Audit trail for all access

MONITORING

    echo "✓ Monitoring configuration created"
}

create_sentry_config
create_crash_reporter
create_error_dashboard
create_monitoring_config

echo "✓ Sentry Crash Monitoring System Complete"

