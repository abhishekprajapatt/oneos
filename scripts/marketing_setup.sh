#!/bin/bash

set -e

DEMO_DIR="marketing/youtube-demos"
ASSETS_DIR="marketing/assets"
SCRIPTS_DIR="marketing/scripts"

mkdir -p "$DEMO_DIR" "$ASSETS_DIR" "$SCRIPTS_DIR"

create_demo_generator() {
    cat > "$DEMO_DIR/demo_generator.sh" <<'DEMO_GEN'
#!/bin/bash

set -e

DEMO_NAME="${1:-oneos-features-demo}"
OUTPUT_DIR="demos/rendered"
SCRIPT_DIR="demos/scripts"
ASSETS_DIR="demos/assets"

mkdir -p "$OUTPUT_DIR" "$SCRIPT_DIR"

create_cross_device_demo() {
    cat > "$SCRIPT_DIR/cross-device.md" <<'SCRIPT'

- Show OneOS running on MacBook Pro
- Desktop showing KDE Connect icon
- Demonstrate Finder integration
- Display system tray with active connections

- iPhone connects to desktop
- KDE Connect popup shows "iPhone connected"
- Clipboard sync demonstration:
  - Copy text on iPhone
  - Paste on desktop
  - Show sync in real-time
- File sharing:
  - Drag photo from iPhone to desktop
  - Show transfer progress
  - Save to Photos app

- Element messaging client demo
- Show conversations on desktop
- Receive message on iPhone
- Notification sync
- Read receipt synchronization
- Show cross-device notification

- Home Assistant dashboard on desktop
- Control lights from OneOS UI
- Show smart speaker interaction
- Adjust thermostat
- Show automation in action
- Mobile control of same devices

- PipeWire system status
- Show ultra-low latency (18.4ms)
- Audio device switching demo
- Bluetooth speaker connection
- Quality visualization

- Feature summary
- Performance metrics overlay
- Download information
- OneOS.dev URL display

- Resolution: 2560x1440 (4K for YouTube)
- Framerate: 60fps
- Aspect Ratio: 16:9
- Music: Royalty-free tech background
- Narration: Professional voiceover
- Duration: Exactly 10 minutes
- Subtitles: English, Spanish, German, Japanese
SCRIPT

    echo "✓ Cross-device demo script created"
}

create_features_demo() {
    cat > "$SCRIPT_DIR/system-features.md" <<'SCRIPT'

- Modern, clean interface
- Dark mode toggle
- Customization options
- Accessibility features

- File Manager navigation
- Calculator demo
- Text Editor features
- Built-in applications overview

- Terminal emulator
- Package manager usage
- Development tools
- Git integration

- System preferences
- Network configuration
- Security settings
- Privacy controls

- Download instructions
- System requirements
- Support resources
- Community links

- Format: WebM, H.264
- Bitrate: 25 Mbps
- Audio: Stereo, 192 kbps
- Upload: 4K (2160p) for best quality
SCRIPT

    echo "✓ Features demo script created"
}

create_install_demo() {
    cat > "$SCRIPT_DIR/installation.md" <<'SCRIPT'

- Welcome
- Version and release date
- System requirements overlay

- Minimum specs
- Recommended specs
- Supported hardware list

- Download from official mirror
- Verify checksum
- Create bootable USB
- BIOS setup (if needed)

- Boot from USB
- Language selection
- Disk partitioning
- Installation progress
- System setup wizard
- User account creation
- Network configuration

- First boot
- Initial setup
- Application installation
- System updates check

- System ready
- Quick tour
- Next steps

- Use actual OneOS installation
- Real hardware or quality virtualization
- Clear audio narration
- On-screen text for key points
SCRIPT

    echo "✓ Installation demo script created"
}

create_performance_demo() {
    cat > "$SCRIPT_DIR/performance.md" <<'SCRIPT'

- Audio latency measurement: 18.4ms
- Real-time waveform visualization
- Comparison chart vs competitors
- Professional audio use case

- Boot time: <15 seconds
- Application launch times
- Multitasking demonstration
- Resource utilization graphs

- 3D graphics test
- Gaming capabilities
- Video playback
- Hardware acceleration

- Web browsing
- Document editing
- Video conferencing
- Productivity showcase

- Performance metrics summary
- Hardware tested
- Reproducibility information

- Performance monitoring tools
- Benchmark software
- Real-world applications
- Comparison data
SCRIPT

    echo "✓ Performance demo script created"
}

generate_all_demos() {
    create_cross_device_demo
    create_features_demo
    create_install_demo
    create_performance_demo
    
    cat > "$OUTPUT_DIR/DEMO_MANIFEST.txt" <<'MANIFEST'

- Title: OneOS Cross-Device Integration - One Device, Connected Life
- Duration: 10 minutes
- Focus: KDE Connect, messaging, smart home
- Audience: Users interested in device connectivity
- SEO Tags: cross-device, sync, messaging, smart home
- CTA: Download from oneos.dev

- Title: OneOS v1.0 Features Tour - Modern Operating System
- Duration: 12 minutes
- Focus: UI, apps, settings, development tools
- Audience: General users, developers
- SEO Tags: operating system, features, applications
- CTA: Visit documentation

- Title: How to Install OneOS v1.0 - Step by Step Guide
- Duration: 8 minutes
- Focus: Installation process, requirements
- Audience: New users, first-time installers
- SEO Tags: installation, setup, tutorial, guide
- CTA: Download ISO from mirrors

- Title: OneOS v1.0 Performance - Blazing Fast OS
- Duration: 6 minutes
- Focus: Audio, speed, graphics, benchmarks
- Audience: Tech enthusiasts, gamers
- SEO Tags: performance, benchmarks, fast, latency
- CTA: Download and test yourself

- Upload to YouTube (main channel)
- Mirror on Vimeo (backup)
- Embed on oneos.dev website
- Share on social media
- Include in documentation
- Use as community educational content

- Branding: Use official OneOS logo
- Resolution: 4K (2160p) minimum
- Subtitles: English (always), add more languages
- Audio: Professional, clear narration
- Music: Royalty-free, tech-themed
- Color Grading: Match OneOS brand
MANIFEST

    echo "✓ Demo manifest created"
}

create_download_tracker() {
    cat > "$SCRIPTS_DIR/analytics_tracker.py" <<'ANALYTICS'
#!/usr/bin/env python3

import json
import sqlite3
from datetime import datetime, timedelta
from collections import defaultdict

class DownloadAnalytics:
    """Track OneOS downloads and milestones"""
    
    def __init__(self, db_path="downloads.db"):
        self.db = sqlite3.connect(db_path)
        self.init_database()
    
    def init_database(self):
        """Create download tracking tables"""
        self.db.execute("""
            CREATE TABLE IF NOT EXISTS downloads (
                id INTEGER PRIMARY KEY,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
                version TEXT,
                source TEXT,
                country TEXT,
                os TEXT,
                ip_hash TEXT UNIQUE,
                referrer TEXT
            )
        """)
        
        self.db.execute("""
            CREATE TABLE IF NOT EXISTS mirrors (
                id INTEGER PRIMARY KEY,
                name TEXT UNIQUE,
                location TEXT,
                bandwidth_mbps INTEGER,
                downloads_today INTEGER DEFAULT 0,
                downloads_total INTEGER DEFAULT 0
            )
        """)
        
        self.db.execute("""
            CREATE TABLE IF NOT EXISTS milestones (
                id INTEGER PRIMARY KEY,
                target INTEGER UNIQUE,
                achieved_at DATETIME,
                announcement_status TEXT DEFAULT 'pending'
            )
        """)
        
        self.db.commit()
    
    def log_download(self, version, source, country, os, ip_hash, referrer=None):
        """Log a download"""
        self.db.execute("""
            INSERT INTO downloads 
            (version, source, country, os, ip_hash, referrer)
            VALUES (?, ?, ?, ?, ?, ?)
        """, (version, source, country, os, ip_hash, referrer))
        
        self.db.commit()
        
        self.check_milestones()
    
    def get_total_downloads(self):
        """Get total download count"""
        cursor = self.db.execute("SELECT COUNT(*) FROM downloads")
        return cursor.fetchone()[0]
    
    def get_downloads_by_version(self):
        """Get downloads by version"""
        cursor = self.db.execute("""
            SELECT version, COUNT(*) as count
            FROM downloads
            GROUP BY version
            ORDER BY version DESC
        """)
        return dict(cursor.fetchall())
    
    def get_downloads_by_country(self, limit=10):
        """Get top countries by downloads"""
        cursor = self.db.execute("""
            SELECT country, COUNT(*) as count
            FROM downloads
            WHERE country IS NOT NULL
            GROUP BY country
            ORDER BY count DESC
            LIMIT ?
        """, (limit,))
        return dict(cursor.fetchall())
    
    def get_downloads_by_source(self):
        """Get downloads by source"""
        cursor = self.db.execute("""
            SELECT source, COUNT(*) as count
            FROM downloads
            GROUP BY source
        """)
        return {
            'torrent': cursor.fetchone()[0] if cursor else 0,
            'direct': cursor.fetchone()[0] if cursor else 0,
            'mirror': cursor.fetchone()[0] if cursor else 0
        }
    
    def get_daily_downloads(self, days=30):
        """Get downloads per day for last N days"""
        cursor = self.db.execute("""
            SELECT DATE(timestamp) as date, COUNT(*) as count
            FROM downloads
            WHERE timestamp > datetime('now', '-' || ? || ' days')
            GROUP BY DATE(timestamp)
            ORDER BY date ASC
        """, (days,))
        return dict(cursor.fetchall())
    
    def check_milestones(self):
        """Check for milestone achievements"""
        total = self.get_total_downloads()
        milestones = [100, 500, 1000, 5000, 10000, 50000, 100000]
        
        for milestone in milestones:
            if total >= milestone:
                cursor = self.db.execute(
                    "SELECT id FROM milestones WHERE target = ?",
                    (milestone,)
                )
                
                if not cursor.fetchone():
                    self.db.execute(
                        "INSERT INTO milestones (target, achieved_at) VALUES (?, ?)",
                        (milestone, datetime.now())
                    )
                    self.db.commit()
                    
                    print(f"🎉 MILESTONE REACHED: {milestone:,} downloads!")
                    return milestone
        
        return None
    
    def generate_report(self):
        """Generate analytics report"""
        total = self.get_total_downloads()
        by_version = self.get_downloads_by_version()
        by_country = self.get_downloads_by_country()
        daily = self.get_daily_downloads()
        
        report = {
            "generated_at": datetime.now().isoformat(),
            "total_downloads": total,
            "milestone": self.get_milestone_progress(),
            "by_version": by_version,
            "top_countries": by_country,
            "daily_trend": daily
        }
        
        return report
    
    def get_milestone_progress(self):
        """Get progress toward 10K milestone"""
        total = self.get_total_downloads()
        target = 10000
        percent = (total / target) * 100
        
        return {
            "current": total,
            "target": target,
            "progress_percent": min(percent, 100),
            "remaining": max(target - total, 0)
        }

if __name__ == "__main__":
    analytics = DownloadAnalytics()
    report = analytics.generate_report()
    
    print(json.dumps(report, indent=2))
ANALYTICS

    chmod +x "$SCRIPTS_DIR/analytics_tracker.py"
    echo "✓ Download analytics tracker created"
}

create_community_kit() {
    cat > "marketing/COMMUNITY_KIT.md" <<'COMMUNITY'

- **Handle**: @OneOS_Project
- **Post Frequency**: 5 posts/week
- **Content Mix**:
  - Development updates (40%)
  - Community highlights (30%)
  - Feature spotlights (20%)
  - Events/announcements (10%)

**Sample Posts**:
```
"🎉 OneOS reached 1,000 downloads! Thank you to our amazing community. Download your copy at oneos.dev

"PipeWire ultra-low latency audio: 18.4ms for professional music production. That's what we call precision.

"Just merged a major performance improvement. Audio latency down by 12%. Check out the PR!
```

- **Frequency**: 2-4 videos per month
- **Content**: Demos, tutorials, announcements
- **Playlists**:
  - Quick Tutorials
  - Demo Videos
  - Developer Talks
  - Community Spotlight

- **Community**: r/OneOS
- **Engagement**: AMA quarterly, weekly discussion threads
- **Content**: Release announcements, feedback surveys

- **Server**: discord.gg/oneos
- **Channels**:
  - announcements (updates)
  - general (chat)
  - help (support)
  - development (technical)
  - showcase (user projects)

- FOSDEM (February)
- Open Source Summit (June)
- LinuxCon (September)
- PyCon/RustConf/etc (subject-specific)

- Monthly community calls
- Regional user groups
- Demo sessions
- Q&A sessions

- Blog posts about interesting setups
- Community spotlight on social media
- Swag recognition
- Early access to features

- Hall of fame on website
- Mention in release notes
- Credit in documentation
- Special roles on Discord

**OneOS Overview**
```
OneOS is a cross-device operating system designed for seamless integration 
across phones, tablets, computers, and smart devices. Version 1.0, released 
January 4, 2026, features KDE Connect integration, ultra-low latency audio 
(18.4ms), full accessibility compliance (WCAG 2.1 AA), and support for 
15 languages.
```

**Contact**: press@oneos.dev
**Website**: oneos.dev
**Repository**: github.com/OneOS/oneos

- Logo files (PNG, SVG)
- Product screenshots
- Demo video clips
- Performance benchmarks
- Team photos

- Quarterly satisfaction surveys
- Feature preference polls
- Hardware compatibility surveys
- Support quality ratings

- Feature request board
- Voting on proposals
- Community input on roadmap
- Transparent decision-making

- Blog post
- Social media announcement
- Thank you video
- Community appreciation

- Major press release
- Video celebration
- Community event (virtual)
- Merchandise giveaway
- Special edition release

- Industry recognition
- Keynote at major conference
- Documentary-style video
- International expansion announcement

- January: New Year, new features
- February: FOSDEM presentations
- March: Spring development updates
- April: Performance improvements
- May: Community spotlight month
- June: Mid-year review
- July: Summer features
- August: Back-to-school educational content
- September: Fall releases
- October: Anniversary celebrations
- November: Year-end gratitude
- December: Community holiday event

COMMUNITY

    echo "✓ Community engagement kit created"
}

generate_all_demos
create_download_tracker
create_community_kit

echo "✓ Marketing & Community Infrastructure Complete"

