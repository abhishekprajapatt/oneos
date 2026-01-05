#!/bin/bash

CACHE_DIR="/var/cache"

clear_all_caches() {
    rm -rf "$CACHE_DIR"/apt/*
    rm -rf "$CACHE_DIR"/fontconfig/*
    rm -rf "$CACHE_DIR"/man/*
    rm -rf "$CACHE_DIR"/package-manager/*
    echo "Caches cleared"
}

clear_cache() {
    local cache_name=$1
    if [ -d "$CACHE_DIR/$cache_name" ]; then
        rm -rf "$CACHE_DIR/$cache_name"/*
        echo "Cleared $cache_name cache"
    fi
}

cache_usage() {
    echo "=== Cache Usage ==="
    du -sh "$CACHE_DIR"/*
}

init_caches() {
    mkdir -p "$CACHE_DIR/apt/archives/partial"
    mkdir -p "$CACHE_DIR/fontconfig"
    mkdir -p "$CACHE_DIR/man"
    mkdir -p "$CACHE_DIR/package-manager"
    chmod -R 755 "$CACHE_DIR"
}

case "$1" in
    init)
        init_caches
        echo "Initialized caches"
        ;;
    clear)
        if [ -z "$2" ]; then
            clear_all_caches
        else
            clear_cache "$2"
        fi
        ;;
    usage)
        cache_usage
        ;;
    *)
        echo "Usage: $0 {init|clear [cache_name]|usage}"
        exit 1
        ;;
esac

