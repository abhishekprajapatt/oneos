#!/bin/bash

set -e

BETA_DIR="beta"
DB_FILE="$BETA_DIR/beta_users.db"
FEEDBACK_DIR="$BETA_DIR/feedback"
BUILD_DIR="builds"

mkdir -p "$BETA_DIR" "$FEEDBACK_DIR" "$BUILD_DIR"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

log() {
    echo -e "${BLUE}[BETA]${NC} $1"
}

success() {
    echo -e "${GREEN}✓${NC} $1"
}

error() {
    echo -e "${RED}✗${NC} $1"
}

warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

init_database() {
    log "Initializing beta user database..."
    
    sqlite3 "$DB_FILE" <<EOF
CREATE TABLE IF NOT EXISTS beta_users (
    id INTEGER PRIMARY KEY,
    email TEXT UNIQUE NOT NULL,
    name TEXT NOT NULL,
    device TEXT NOT NULL,
    os TEXT NOT NULL,
    joined_date DATETIME DEFAULT CURRENT_TIMESTAMP,
    status TEXT DEFAULT 'active',
    feedback_count INTEGER DEFAULT 0,
    issues_reported INTEGER DEFAULT 0,
    satisfaction_score REAL DEFAULT 0,
    nda_signed INTEGER DEFAULT 0,
    tier TEXT DEFAULT 'standard'
);

CREATE TABLE IF NOT EXISTS feedback (
    id INTEGER PRIMARY KEY,
    user_id INTEGER NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    category TEXT NOT NULL,
    rating INTEGER,
    comment TEXT,
    component TEXT,
    FOREIGN KEY (user_id) REFERENCES beta_users(id)
);

CREATE TABLE IF NOT EXISTS beta_builds (
    id INTEGER PRIMARY KEY,
    version TEXT UNIQUE NOT NULL,
    release_date DATETIME DEFAULT CURRENT_TIMESTAMP,
    status TEXT DEFAULT 'testing',
    testers_count INTEGER DEFAULT 0,
    issues_found INTEGER DEFAULT 0,
    pass_rate REAL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS build_compatibility (
    id INTEGER PRIMARY KEY,
    build_id INTEGER NOT NULL,
    user_id INTEGER NOT NULL,
    compatible INTEGER DEFAULT 1,
    issues TEXT,
    FOREIGN KEY (build_id) REFERENCES beta_builds(id),
    FOREIGN KEY (user_id) REFERENCES beta_users(id)
);

CREATE TABLE IF NOT EXISTS feature_requests (
    id INTEGER PRIMARY KEY,
    user_id INTEGER NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    title TEXT NOT NULL,
    description TEXT,
    priority TEXT DEFAULT 'normal',
    votes INTEGER DEFAULT 0,
    FOREIGN KEY (user_id) REFERENCES beta_users(id)
);
EOF
    success "Database initialized: $DB_FILE"
}

add_beta_user() {
    local email=$1
    local name=$2
    local device=$3
    local os=$4
    
    log "Adding beta user: $email"
    
    sqlite3 "$DB_FILE" <<EOF
INSERT INTO beta_users (email, name, device, os) 
VALUES ('$email', '$name', '$device', '$os');
EOF
    
    success "User added: $email"
    
    send_welcome_email "$email" "$name"
}

send_welcome_email() {
    local email=$1
    local name=$2
    
    cat > "$FEEDBACK_DIR/welcome_${email}.txt" <<EOF
Subject: Welcome to OneOS Beta Program

Dear $name,

Welcome to the OneOS Beta Program! You're among 100 selected testers to help us refine this cutting-edge operating system.

Your Mission:
- Test core features and integrations
- Report bugs with detailed reproduction steps
- Share your experience and suggestions
- Help us achieve 95% stability before launch

Your Tier Benefits:
- Early access to new features
- Direct communication with development team
- Exclusive beta swag
- Recognition in release credits

Quick Start:
1. Download: oneos.dev/beta/builds
2. Install: Follow the setup guide
3. Report Bugs: GitHub Issues (label: beta-feedback)
4. Share Feedback: Feedback form at oneos.dev/beta

Key Areas to Test:
1. Cross-device synchronization (KDE Connect)
2. Wireless audio (LibrePods, PipeWire)
3. Unified messaging (Element bridge)
4. Smart home control (Home Assistant)
5. Accessibility (WCAG compliance)
6. Performance benchmarks
7. Battery impact on mobile
8. Security features

Testing Guidelines:
- Test on your primary device
- Document reproduction steps for bugs
- Include system information in reports
- Rate features you test (1-10)
- Provide actionable feedback

NDA Reminder:
OneOS beta features are confidential until public release. Please do not:
- Share builds outside the beta program
- Discuss specific features on social media
- Post screenshots online without permission

Support:
Questions? Email: beta@oneos.dev
Bug Reports: https:
Community: https:

Thank you for being part of this journey!

Best regards,
The OneOS Team
EOF
    
    success "Welcome email template created for $email"
}

list_beta_users() {
    log "Beta Users (Total: 100 max)"
    echo ""
    sqlite3 -header -column "$DB_FILE" <<EOF
SELECT 
    id,
    name,
    device,
    os,
    status,
    feedback_count,
    issues_reported,
    ROUND(satisfaction_score, 1) as satisfaction
FROM beta_users
ORDER BY joined_date DESC;
EOF
}

get_user_stats() {
    log "Beta User Statistics"
    echo ""
    
    sqlite3 "$DB_FILE" <<EOF
SELECT 'Total Users: ' || COUNT(*) FROM beta_users;
SELECT 'Active Users: ' || COUNT(*) FROM beta_users WHERE status = 'active';
SELECT 'Inactive Users: ' || COUNT(*) FROM beta_users WHERE status = 'inactive';
SELECT 'Avg Feedback Score: ' || ROUND(AVG(satisfaction_score), 2) FROM beta_users;
SELECT 'Total Issues Reported: ' || SUM(issues_reported) FROM beta_users;
SELECT 'Total Feedback Items: ' || SUM(feedback_count) FROM beta_users;
EOF
}

submit_feedback() {
    local user_id=$1
    local category=$2
    local rating=$3
    local comment=$4
    local component=$5
    
    log "Recording feedback from user $user_id"
    
    sqlite3 "$DB_FILE" <<EOF
INSERT INTO feedback (user_id, category, rating, comment, component)
VALUES ($user_id, '$category', $rating, '$comment', '$component');

UPDATE beta_users SET feedback_count = feedback_count + 1
WHERE id = $user_id;
EOF
    
    success "Feedback recorded"
}

process_feedback_form() {
    local json_file=$1
    
    user_id=$(jq -r '.user_id' "$json_file")
    category=$(jq -r '.category' "$json_file")
    rating=$(jq -r '.rating' "$json_file")
    comment=$(jq -r '.comment' "$json_file")
    component=$(jq -r '.component' "$json_file")
    
    submit_feedback "$user_id" "$category" "$rating" "$comment" "$component"
}

analyze_feedback() {
    log "Analyzing Beta Feedback"
    echo ""
    
    echo -e "${CYAN}Feedback by Category:${NC}"
    sqlite3 "$DB_FILE" <<EOF
SELECT category, COUNT(*) as count, ROUND(AVG(rating), 2) as avg_rating
FROM feedback
GROUP BY category
ORDER BY count DESC;
EOF
    
    echo ""
    echo -e "${CYAN}Component Issues:${NC}"
    sqlite3 "$DB_FILE" <<EOF
SELECT component, COUNT(*) as issues, ROUND(AVG(rating), 2) as avg_rating
FROM feedback
WHERE rating < 7
GROUP BY component
ORDER BY issues DESC;
EOF
    
    echo ""
    echo -e "${CYAN}Top Feature Requests:${NC}"
    sqlite3 "$DB_FILE" <<EOF
SELECT title, votes, COUNT(*) as requests
FROM feature_requests
GROUP BY title
ORDER BY votes DESC
LIMIT 10;
EOF
}

create_beta_build() {
    local version=$1
    
    log "Creating beta build: $version"
    
    make -f Makefile.integration all
    
    bash scripts/integration_test_suite.sh > /tmp/test_results.log 2>&1
    
    PASS_RATE=$(tail -5 /tmp/test_results.log | grep -oP '\d+\.\d+(?=%)')
    
    sqlite3 "$DB_FILE" <<EOF
INSERT INTO beta_builds (version, status, pass_rate)
VALUES ('$version', 'ready', $PASS_RATE);
EOF
    
    mkdir -p "$BUILD_DIR/$version"
    cp build/oneos.iso "$BUILD_DIR/$version/"
    cp HARDWARE_COMPATIBILITY.md "$BUILD_DIR/$version/"
    cp BUG_TRACKING_CONFIG.md "$BUILD_DIR/$version/"
    
    cat > "$BUILD_DIR/$version/RELEASE_NOTES.md" <<EOF

**Release Date:** $(date)
**Test Pass Rate:** ${PASS_RATE}%
**Status:** Ready for Testing

- Cross-device integration (Phase 4)
- Smart home control
- Unified messaging
- Ultra-low latency audio

1. Compatibility across 20+ devices
2. Performance benchmarks
3. Accessibility compliance
4. Security audits
5. Battery impact assessment

- See KNOWN_ISSUES.md

Use: https:
Label: beta-feedback

Version: $version
Build Date: $(date)
Test Pass Rate: ${PASS_RATE}%
Supported Devices: 20+
EOF
    
    success "Beta build created: $BUILD_DIR/$version"
}

release_to_testers() {
    local version=$1
    
    log "Releasing build $version to beta testers"
    
    sqlite3 "$DB_FILE" <<EOF
UPDATE beta_builds 
SET status = 'released', testers_count = (SELECT COUNT(*) FROM beta_users WHERE status = 'active')
WHERE version = '$version';
EOF
    
    TESTER_COUNT=$(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM beta_users WHERE status = 'active';")
    
    cat > "$FEEDBACK_DIR/release_notification_$version.txt" <<EOF
New Beta Build Available: $version

A new beta build is ready for testing!

Download: https:
Release Notes: https:

Installation:
1. Download the ISO
2. Follow: INSTALLATION_GUIDE.md
3. Test the key features
4. Report any issues

Timeline:
- Testing period: 2 weeks
- Feedback deadline: 10 days
- Next build: TBD

Thank you for your participation!

The OneOS Team
EOF
    
    success "Build $version released to $TESTER_COUNT testers"
}

sync_issues_to_github() {
    log "Syncing beta issues to GitHub"
    
    sqlite3 "$DB_FILE" <<EOF | while read issue; do
        gh issue create \
            --title "$issue" \
            --label "beta-feedback" \
            --label "triage"
    done
    
    success "Issues synced to GitHub"
}

generate_weekly_report() {
    local week=$1
    
    log "Generating weekly beta report for week $week"
    
    cat > "$BETA_DIR/report_week_$week.md" <<EOF

**Report Date:** $(date)
**Period:** Week $week
**Active Testers:** $(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM beta_users WHERE status = 'active';")

$(sqlite3 "$DB_FILE" <<EOSQL
SELECT 'Feedback Submissions: ' || COUNT(*) FROM feedback 
WHERE timestamp > datetime('now', '-7 days');
EOSQL
)

$(sqlite3 "$DB_DIR" <<EOSQL
SELECT 'New Issues: ' || COUNT(*) FROM feedback 
WHERE rating < 5 AND timestamp > datetime('now', '-7 days');
EOSQL
)

$(sqlite3 "$DB_FILE" <<EOSQL
SELECT 'Average Rating: ' || ROUND(AVG(rating), 2) FROM feedback
WHERE timestamp > datetime('now', '-7 days');
EOSQL
)

$(sqlite3 "$DB_FILE" <<EOSQL
SELECT '- ' || component || ': ' || COUNT(*) || ' reports' FROM feedback
WHERE timestamp > datetime('now', '-7 days') AND rating < 5
GROUP BY component
ORDER BY COUNT(*) DESC
LIMIT 5;
EOSQL
)

$(sqlite3 "$DB_FILE" <<EOSQL
SELECT '- ' || comment FROM feedback
WHERE timestamp > datetime('now', '-7 days') AND rating >= 8
ORDER BY timestamp DESC
LIMIT 3;
EOSQL
)

$(sqlite3 "$DB_FILE" <<EOSQL
SELECT '- ' || component || ': ' || comment FROM feedback
WHERE timestamp > datetime('now', '-7 days') AND rating < 5
ORDER BY timestamp DESC
LIMIT 3;
EOSQL
)

Top 5 most requested features:
$(sqlite3 "$DB_FILE" <<EOSQL
SELECT ROW_NUMBER() OVER (ORDER BY votes DESC) || '. ' || title || ' (' || votes || ' votes)'
FROM feature_requests
ORDER BY votes DESC
LIMIT 5;
EOSQL
)

- [ ] Review critical feedback
- [ ] Prioritize issues for next sprint
- [ ] Prepare next beta build
- [ ] Update documentation based on user feedback
- [ ] Schedule 1:1 interviews with power users

---

*Generated for OneOS Beta Program*
*Next Report: $(date -d "next week" "+%Y-%m-%d")*

EOF
    
    success "Weekly report generated: $BETA_DIR/report_week_$week.md"
}

show_menu() {
    echo ""
    echo -e "${CYAN}OneOS Beta Program Manager${NC}"
    echo "1. Initialize database"
    echo "2. Add beta user"
    echo "3. List beta users"
    echo "4. User statistics"
    echo "5. Submit feedback"
    echo "6. Analyze feedback"
    echo "7. Create beta build"
    echo "8. Release to testers"
    echo "9. Generate weekly report"
    echo "0. Exit"
    echo ""
}

if [ $
    show_menu
    read -p "Select option: " option
    
    case $option in
        1) init_database ;;
        2) 
            read -p "Email: " email
            read -p "Name: " name
            read -p "Device: " device
            read -p "OS: " os
            add_beta_user "$email" "$name" "$device" "$os"
            ;;
        3) list_beta_users ;;
        4) get_user_stats ;;
        5) 
            read -p "User ID: " uid
            read -p "Category: " category
            read -p "Rating (1-10): " rating
            read -p "Comment: " comment
            read -p "Component: " component
            submit_feedback "$uid" "$category" "$rating" "$comment" "$component"
            ;;
        6) analyze_feedback ;;
        7)
            read -p "Version (e.g., beta-1.0.0): " version
            create_beta_build "$version"
            ;;
        8)
            read -p "Version to release: " version
            release_to_testers "$version"
            ;;
        9)
            read -p "Week number: " week
            generate_weekly_report "$week"
            ;;
        0) exit 0 ;;
        *) error "Invalid option" ;;
    esac
else
    case $1 in
        init) init_database ;;
        add) add_beta_user "$2" "$3" "$4" "$5" ;;
        list) list_beta_users ;;
        stats) get_user_stats ;;
        analyze) analyze_feedback ;;
        build) create_beta_build "$2" ;;
        release) release_to_testers "$2" ;;
        report) generate_weekly_report "$2" ;;
        *) echo "Usage: $0 {init|add|list|stats|analyze|build|release|report}" ;;
    esac
fi

