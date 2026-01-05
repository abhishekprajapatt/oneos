#!/bin/bash

apt_cache_manager() {
    case "$1" in
        clean)
            rm -f /var/cache/apt/archives/*.deb
            echo "Cleaned APT cache"
            ;;
        size)
            du -sh /var/cache/apt/
            ;;
        *)
            echo "Usage: apt_cache_manager {clean|size}"
            ;;
    esac
}

fontconfig_cache_manager() {
    case "$1" in
        clean)
            rm -rf /var/cache/fontconfig/*
            echo "Cleaned fontconfig cache"
            ;;
        rebuild)
            fc-cache -f -v
            echo "Rebuilt fontconfig cache"
            ;;
        *)
            echo "Usage: fontconfig_cache_manager {clean|rebuild}"
            ;;
    esac
}

man_cache_manager() {
    case "$1" in
        update)
            mandb /var/cache/man
            echo "Updated man cache"
            ;;
        clean)
            rm -rf /var/cache/man/*
            echo "Cleaned man cache"
            ;;
        *)
            echo "Usage: man_cache_manager {update|clean}"
            ;;
    esac
}

case "$1" in
    apt)
        apt_cache_manager "$2"
        ;;
    fontconfig)
        fontconfig_cache_manager "$2"
        ;;
    man)
        man_cache_manager "$2"
        ;;
    *)
        echo "Usage: $0 {apt|fontconfig|man} <command>"
        ;;
esac

