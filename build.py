#!/usr/bin/env python3

import os
import sys
import subprocess
import time

def run_command(cmd, description=""):
    try:
        result = subprocess.run(cmd, shell=True, capture_output=False)
        return result.returncode == 0
    except Exception as e:
        print(f"[!] Error: {e}")
        return False

def main():
    print("\n" + "="*50)
    print("OneOS 40-Part Architecture")
    print("Complete Build System")
    print("="*50)
    
    start_time = time.time()
    
    # Check prerequisites
    print("\n[1] Checking prerequisites...")
    required_tools = ["gcc", "make", "ld", "objcopy"]
    for tool in required_tools:
        if os.system(f"which {tool} > /dev/null 2>&1") != 0:
            print(f"[!] Error: {tool} not found")
            sys.exit(1)
        print(f"    ✓ {tool} found")
    
    # Run build
    print("\n[2] Starting build...")
    if not run_command("bash build.sh"):
        print("\n[!] Build failed")
        sys.exit(1)
    
    elapsed = time.time() - start_time
    
    print("\n" + "="*50)
    print(f"Build completed in {elapsed:.1f}s")
    print("="*50)
    print("\nArtifacts:")
    
    artifacts = [
        "oneos-kernel.elf",
        "oneos-kernel.bin",
        "oneos.iso",
        "kernel.map"
    ]
    
    for artifact in artifacts:
        if os.path.exists(artifact):
            size = os.path.getsize(artifact)
            if size > 1024*1024:
                print(f"  ✓ {artifact} ({size/(1024*1024):.2f} MB)")
            else:
                print(f"  ✓ {artifact} ({size/1024:.2f} KB)")

if __name__ == "__main__":
    main()
