#!/bin/bash

THEME_CONFIG="/etc/oneos/theme.conf"
CURRENT_THEME_FILE="/var/run/oneos_current_theme"

HOUR=$(date +%H)

determine_theme() {
    if [ "$HOUR" -ge 6 ] && [ "$HOUR" -lt 18 ]; then
        echo "light"
    else
        echo "dark"
    fi
}

apply_theme() {
    local THEME=$1
    
    if [ "$THEME" = "light" ]; then
        gsettings set org.gnome.desktop.interface gtk-theme 'WhiteSur'
        gsettings set org.gnome.desktop.interface icon-theme 'WhiteSur'
        gsettings set org.gnome.desktop.interface color-scheme 'light'
        
        gsettings set org.gnome.desktop.background picture-uri-dark \
            'file:
        
        gsettings set org.gnome.desktop.interface text-scaling-factor 1.0
        
    elif [ "$THEME" = "dark" ]; then
        gsettings set org.gnome.desktop.interface gtk-theme 'WhiteSur-dark'
        gsettings set org.gnome.desktop.interface icon-theme 'WhiteSur-dark'
        gsettings set org.gnome.desktop.interface color-scheme 'dark'
        
        gsettings set org.gnome.desktop.background picture-uri-dark \
            'file:
    fi
    
    echo "$THEME" > "$CURRENT_THEME_FILE"
    
    notify_theme_change "$THEME"
}

notify_theme_change() {
    local THEME=$1
    
    dbus-send --system /org/oneos/ThemeManager \
        org.oneos.ThemeManager.ThemeChanged \
        string:"$THEME"
    
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] Theme changed to: $THEME" \
        >> /var/log/oneos_theme.log
}

DESIRED_THEME=$(determine_theme)
CURRENT_THEME=""

if [ -f "$CURRENT_THEME_FILE" ]; then
    CURRENT_THEME=$(cat "$CURRENT_THEME_FILE")
fi

if [ "$DESIRED_THEME" != "$CURRENT_THEME" ]; then
    apply_theme "$DESIRED_THEME"
fi

exit 0

