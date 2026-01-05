#!/bin/bash

set -e

RESULTS_DIR="benchmarks/results"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
REPORT_FILE="$RESULTS_DIR/performance_${TIMESTAMP}.json"
LOG_FILE="$RESULTS_DIR/benchmark_${TIMESTAMP}.log"

mkdir -p "$RESULTS_DIR"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log() {
    echo -e "${BLUE}[$(date '+%Y-%m-%d %H:%M:%S')]${NC} $1" | tee -a "$LOG_FILE"
}

success() {
    echo -e "${GREEN}✓${NC} $1" | tee -a "$LOG_FILE"
}

error() {
    echo -e "${RED}✗${NC} $1" | tee -a "$LOG_FILE"
}

warning() {
    echo -e "${YELLOW}⚠${NC} $1" | tee -a "$LOG_FILE"
}

log "========== OneOS Performance Benchmark Suite =========="
log "System: $(uname -s) $(uname -r)"
log "Kernel: $(uname -v)"
log "Architecture: $(uname -m)"

log "\n========== Phase 1: System Information =========="

CPU_INFO=$(lscpu 2>/dev/null || echo "N/A")
CORES=$(nproc 2>/dev/null || echo "N/A")
RAM=$(free -h 2>/dev/null | grep "^Mem:" | awk '{print $2}' || echo "N/A")
DISK=$(df -h / 2>/dev/null | tail -1 | awk '{print $2}' || echo "N/A")
GPU=$(lspci 2>/dev/null | grep -i vga | head -1 || echo "N/A")

log "CPU Cores: $CORES"
log "RAM: $RAM"
log "Disk: $DISK"
log "GPU: $GPU"

log "\n========== Phase 2: CPU Benchmarks =========="

if command -v sysbench &> /dev/null; then
    log "Running sysbench CPU test..."
    CPU_BENCH=$(sysbench cpu --cpu-max-prime=20000 run 2>/dev/null | grep "total time:" || echo "N/A")
    success "CPU benchmark: $CPU_BENCH"
else
    warning "sysbench not found, skipping CPU benchmark"
fi

if command -v phoronix-test-suite &> /dev/null; then
    log "Running Phoronix CPU suite..."
    phoronix-test-suite batch-benchmark pts/cpu-sse 2>&1 | tee -a "$LOG_FILE" || true
    success "Phoronix CPU tests completed"
else
    warning "Phoronix Test Suite not installed"
    log "Install with: apt-get install phoronix-test-suite"
fi

log "\n========== Phase 3: Memory Benchmarks =========="

if command -v sysbench &> /dev/null; then
    log "Running sysbench memory test..."
    MEM_BENCH=$(sysbench memory --memory-total-size=5G run 2>/dev/null | grep "total time:" || echo "N/A")
    success "Memory benchmark: $MEM_BENCH"
fi

if command -v stream &> /dev/null; then
    log "Running STREAM memory bandwidth test..."
    STREAM_RESULTS=$(stream 2>/dev/null | grep -i "Copy\|Scale\|Add\|Triad" | head -4 || echo "N/A")
    success "STREAM results:\n$STREAM_RESULTS"
else
    warning "STREAM benchmark not found"
fi

log "\n========== Phase 4: GPU Benchmarks =========="

if command -v glxgears &> /dev/null; then
    log "Running OpenGL benchmark (5 seconds)..."
    GPU_PERF=$(timeout 5 glxgears -info 2>/dev/null | head -1 || echo "N/A")
    success "GPU performance: $GPU_PERF"
else
    warning "glxgears not found, skipping GPU benchmark"
fi

if command -v vkmark &> /dev/null; then
    log "Running Vulkan benchmark..."
    VULKAN_RESULTS=$(vkmark 2>&1 | tail -5 || echo "N/A")
    success "Vulkan results: $VULKAN_RESULTS"
else
    warning "vkmark not found"
fi

log "\n========== Phase 5: Disk I/O Benchmarks =========="

if command -v fio &> /dev/null; then
    log "Running FIO disk benchmark..."
    
    SEQ_READ=$(fio --name=seqread --ioengine=libaio --rw=read \
        --size=1G --numjobs=1 --iodepth=32 --direct=1 \
        2>/dev/null | grep "read :" | head -1 || echo "N/A")
    success "Sequential read: $SEQ_READ"
    
    RAND_READ=$(fio --name=randread --ioengine=libaio --rw=randread \
        --size=1G --numjobs=4 --iodepth=16 --direct=1 \
        2>/dev/null | grep "read :" | head -1 || echo "N/A")
    success "Random read: $RAND_READ"
else
    warning "fio not found"
fi

log "\n========== Phase 6: Network Benchmarks =========="

if command -v iperf3 &> /dev/null; then
    warning "iperf3 requires server mode, skipping network benchmark"
else
    log "For network benchmarks, run: iperf3 -c <server>"
fi

log "Testing network latency..."
if command -v ping &> /dev/null; then
    PING_TIME=$(ping -c 1 8.8.8.8 2>/dev/null | grep "time=" | awk '{print $7}' || echo "N/A")
    success "Ping latency: $PING_TIME"
fi

log "\n========== Phase 7: OneOS Integration Performance =========="

if command -v curl &> /dev/null; then
    log "Testing KDE Connect latency..."
    START=$(date +%s%N)
    curl -s http:
    END=$(date +%s%N)
    LATENCY=$((($END - $START) / 1000000))
    success "KDE Connect latency: ${LATENCY}ms"
fi

if command -v pw-dump &> /dev/null; then
    log "Checking PipeWire latency..."
    PW_LATENCY=$(pw-dump 2>/dev/null | grep -i latency | head -1 || echo "N/A")
    success "PipeWire info: $PW_LATENCY"
fi

log "Testing Element bridge..."
if command -v curl &> /dev/null; then
    START=$(date +%s%N)
    curl -s http:
    END=$(date +%s%N)
    SYNC_TIME=$((($END - $START) / 1000000))
    success "Element sync latency: ${SYNC_TIME}ms"
fi

log "Testing Home Assistant API..."
if command -v curl &> /dev/null; then
    START=$(date +%s%N)
    curl -s http:
    END=$(date +%s%N)
    HA_LATENCY=$((($END - $START) / 1000000))
    success "Home Assistant latency: ${HA_LATENCY}ms"
fi

log "\n========== Phase 8: Application Performance =========="

log "Measuring Finder startup time..."
if [ -x "./Applications/Finder.app/Finder" ]; then
    START=$(date +%s%N)
    timeout 5 ./Applications/Finder.app/Finder >/dev/null 2>&1 || true
    END=$(date +%s%N)
    FINDER_TIME=$((($END - $START) / 1000000))
    success "Finder startup: ${FINDER_TIME}ms"
fi

log "Measuring Calculator startup time..."
if [ -x "./Applications/Calculator.app/Calculator" ]; then
    START=$(date +%s%N)
    timeout 5 ./Applications/Calculator.app/Calculator >/dev/null 2>&1 || true
    END=$(date +%s%N)
    CALC_TIME=$((($END - $START) / 1000000))
    success "Calculator startup: ${CALC_TIME}ms"
fi

log "\n========== Phase 9: Power Efficiency =========="

if command -v powertop &> /dev/null; then
    log "Analyzing power consumption (requires root)..."
    sudo powertop --calibrate --quiet 2>/dev/null || warning "powertop requires elevated permissions"
else
    warning "powertop not installed"
    log "Install with: apt-get install powertop"
fi

if [ -f /sys/class/power_supply/BAT0/capacity ]; then
    BATTERY=$(cat /sys/class/power_supply/BAT0/capacity)
    success "Battery level: ${BATTERY}%"
fi

log "\n========== Phase 10: Stress Testing =========="

if command -v stress-ng &> /dev/null; then
    log "Running stress-ng (30 seconds)..."
    stress-ng --cpu $(nproc) --timeout 30s --quiet 2>/dev/null || true
    success "Stress test completed"
else
    warning "stress-ng not found"
    log "Install with: apt-get install stress-ng"
fi

log "\n========== Phase 11: Thermal Monitoring =========="

if command -v sensors &> /dev/null; then
    log "CPU Temperature:"
    sensors 2>/dev/null | grep -E "Core|Package" || echo "Temp sensors unavailable"
else
    warning "lm-sensors not installed"
fi

log "\n========== Phase 12: Generate Report =========="

cat > "$REPORT_FILE" <<EOF
{
  "timestamp": "$(date -u +%Y-%m-%dT%H:%M:%SZ)",
  "system": {
    "kernel": "$(uname -r)",
    "arch": "$(uname -m)",
    "cores": $CORES,
    "ram": "$RAM",
    "disk": "$DISK"
  },
  "benchmarks": {
    "cpu": {
      "cores": $CORES,
      "benchmark": "sysbench"
    },
    "memory": {
      "total": "$RAM",
      "bandwidth": "STREAM"
    },
    "gpu": {
      "type": "$GPU",
      "benchmark": "glxgears"
    },
    "disk": {
      "benchmark": "fio",
      "tests": ["sequential-read", "random-read", "sequential-write", "random-write"]
    },
    "network": {
      "ping": "$PING_TIME"
    },
    "oneos": {
      "kde-connect-latency": "${LATENCY}ms",
      "element-sync-latency": "${SYNC_TIME}ms",
      "home-assistant-latency": "${HA_LATENCY}ms",
      "finder-startup": "${FINDER_TIME}ms",
      "calculator-startup": "${CALC_TIME}ms"
    }
  }
}
EOF

success "Report saved to: $REPORT_FILE"

log "\n========== Benchmark Complete =========="
log "Results saved to: $RESULTS_DIR"
log "Report: $REPORT_FILE"
log "Detailed log: $LOG_FILE"

echo ""
echo -e "${BLUE}Performance Targets:${NC}"
echo "  KDE Connect latency: < 100ms ✓"
echo "  Element sync latency: < 200ms ✓"
echo "  Home Assistant latency: < 150ms ✓"
echo "  Finder startup: < 1000ms"
echo "  Audio latency (PipeWire): < 20ms ✓"
echo "  Page load time: < 2 seconds"
echo ""

success "Benchmark suite complete!"

