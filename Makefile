KERNEL_DIR := kernel
DRIVERS_DIR := drivers
BIN_DIR := bin
FS_DIR := fs
ARCH_DIR := arch
BUILD_DIR := build
OUTPUT_DIR := $(BUILD_DIR)/output
UI_DIR := ui-system
CORE_SERVICES_DIR := core-services
NET_DIR := net
SECURITY_DIR := security
COMPAT_DIR := compatibility-layer
APPS_DIR := Applications
PKG_DIR := package-managers
BOOT_DIR := boot
MM_DIR := mm

CC := gcc
CFLAGS := -Wall -Wextra -O2 -fPIC -I$(PWD)/include -I$(PWD)/ui-system -I$(PWD)/kernel -I$(PWD)/core-services -I$(PWD)/mm -I$(PWD)/fs -I$(PWD)/net -I$(PWD)/security -I$(PWD)/drivers -I$(PWD)/compatibility-layer -I$(PWD)/Applications -I$(PWD)/package-managers -I$(PWD)/boot
LDFLAGS := -L$(OUTPUT_DIR) -lpthread

KERNEL_OBJS := $(KERNEL_DIR)/*.o
DRIVER_OBJS := $(DRIVERS_DIR)/**/*.o
BIN_OBJS := $(BIN_DIR)/**/*.o
FS_OBJS := $(FS_DIR)/**/*.o
UI_OBJS := $(UI_DIR)/*.o
CORE_SVC_OBJS := $(CORE_SERVICES_DIR)/*.o
NET_OBJS := $(NET_DIR)/*.o
SECURITY_OBJS := $(SECURITY_DIR)/*.o
COMPAT_OBJS := $(COMPAT_DIR)/*.o
APPS_OBJS := $(APPS_DIR)/*.o
PKG_OBJS := $(PKG_DIR)/*.o
BOOT_OBJS := $(BOOT_DIR)/*.o
MM_OBJS := $(MM_DIR)/*.o

MAIN_OBJS := oneos_main.o oneos.o

.PHONY: all clean build drivers kernel bin fs arch ui core-services net security compat apps packages boot services tools mm enterprise advanced observability part8 part9 part10 part11 part12

all: build ui core-services mm fs net drivers security compat boot apps packages kernel bin arch services tools enterprise advanced observability part8 part9 part10 part11 part12 oneos

kernel:
	@echo "Building kernel..."
	$(CC) $(CFLAGS) -c $(KERNEL_DIR)/kernel_hybrid.c -o $(OUTPUT_DIR)/kernel_hybrid.o
	$(CC) $(CFLAGS) -c $(KERNEL_DIR)/interrupt_handler.c -o $(OUTPUT_DIR)/interrupt_handler.o
	$(CC) $(CFLAGS) -c $(KERNEL_DIR)/syscall_handler.c -o $(OUTPUT_DIR)/syscall_handler.o
	$(CC) $(CFLAGS) -c $(KERNEL_DIR)/realtime.c -o $(OUTPUT_DIR)/realtime.o

drivers:
	@echo "Building drivers..."
	$(MAKE) -C $(DRIVERS_DIR)

bin:
	@echo "Building bin utilities..."
	$(MAKE) -C $(BIN_DIR)

fs:
	@echo "Building filesystem..."
	$(CC) $(CFLAGS) -c $(FS_DIR)/filesystem_core.c -o $(OUTPUT_DIR)/filesystem_core.o
	$(CC) $(CFLAGS) -c $(FS_DIR)/advanced_filesystem.c -o $(OUTPUT_DIR)/advanced_filesystem.o
	$(CC) $(CFLAGS) -c $(FS_DIR)/fs_journal.c -o $(OUTPUT_DIR)/fs_journal.o

arch:
	@echo "Building architecture modules..."
	$(MAKE) -C $(ARCH_DIR)

ui:
	@echo "Compiling UI system..."
	$(CC) $(CFLAGS) -c $(UI_DIR)/ui_core.c -o $(OUTPUT_DIR)/ui_core.o
	$(CC) $(CFLAGS) -c $(UI_DIR)/menu_bar.c -o $(OUTPUT_DIR)/menu_bar.o
	$(CC) $(CFLAGS) -c $(UI_DIR)/dock.c -o $(OUTPUT_DIR)/dock.o
	$(CC) $(CFLAGS) -c $(UI_DIR)/window_manager.c -o $(OUTPUT_DIR)/window_manager.o
	$(CC) $(CFLAGS) -c $(UI_DIR)/display_server.c -o $(OUTPUT_DIR)/display_server.o
	$(CC) $(CFLAGS) -c $(UI_DIR)/input_manager.c -o $(OUTPUT_DIR)/input_manager.o
	$(CC) $(CFLAGS) -c $(UI_DIR)/graphics_core.c -o $(OUTPUT_DIR)/graphics_core.o
	$(CC) $(CFLAGS) -c $(UI_DIR)/audio_engine.c -o $(OUTPUT_DIR)/audio_engine.o

core-services:
	@echo "Building core services..."
	$(CC) $(CFLAGS) -c $(CORE_SERVICES_DIR)/process_scheduler.c -o $(OUTPUT_DIR)/process_scheduler.o
	$(CC) $(CFLAGS) -c $(CORE_SERVICES_DIR)/ipc_manager.c -o $(OUTPUT_DIR)/ipc_manager.o
	$(CC) $(CFLAGS) -c $(CORE_SERVICES_DIR)/thread_manager.c -o $(OUTPUT_DIR)/thread_manager.o
	$(CC) $(CFLAGS) -c $(CORE_SERVICES_DIR)/system_logger.c -o $(OUTPUT_DIR)/system_logger.o
	$(CC) $(CFLAGS) -c $(CORE_SERVICES_DIR)/advanced_scheduler.c -o $(OUTPUT_DIR)/advanced_scheduler.o

mm:
	@echo "Building memory manager..."
	$(CC) $(CFLAGS) -c $(MM_DIR)/memory_manager.c -o $(OUTPUT_DIR)/memory_manager.o
	$(CC) $(CFLAGS) -c $(MM_DIR)/virtual_memory.c -o $(OUTPUT_DIR)/virtual_memory.o
	$(CC) $(CFLAGS) -c $(MM_DIR)/memory_optimization.c -o $(OUTPUT_DIR)/memory_optimization.o

net:
	@echo "Building networking..."
	$(CC) $(CFLAGS) -c $(NET_DIR)/networking_core.c -o $(OUTPUT_DIR)/networking_core.o
	$(CC) $(CFLAGS) -c $(NET_DIR)/network_stack.c -o $(OUTPUT_DIR)/network_stack.o

security:
	@echo "Building security..."
	$(CC) $(CFLAGS) -c $(SECURITY_DIR)/security_manager.c -o $(OUTPUT_DIR)/security_manager.o
	$(CC) $(CFLAGS) -c $(SECURITY_DIR)/security_core.c -o $(OUTPUT_DIR)/security_core.o

compat:
	@echo "Building compatibility layer..."
	$(CC) $(CFLAGS) -c $(COMPAT_DIR)/compatibility_layer.c -o $(OUTPUT_DIR)/compatibility_layer.o

apps:
	@echo "Building application manager..."
	$(CC) $(CFLAGS) -c $(APPS_DIR)/application_manager.c -o $(OUTPUT_DIR)/application_manager.o

packages:
	@echo "Building package manager..."
	$(CC) $(CFLAGS) -c $(PKG_DIR)/package_manager.c -o $(OUTPUT_DIR)/package_manager.o

boot:
	@echo "Building boot manager..."
	$(CC) $(CFLAGS) -c $(BOOT_DIR)/boot_manager.c -o $(OUTPUT_DIR)/boot_manager.o

services:
	@echo "Building system services..."
	$(CC) $(CFLAGS) -c services/power_manager.c -o $(OUTPUT_DIR)/power_manager.o
	$(CC) $(CFLAGS) -c services/virtualization.c -o $(OUTPUT_DIR)/virtualization.o
	$(CC) $(CFLAGS) -c services/container_manager.c -o $(OUTPUT_DIR)/container_manager.o

tools:
	@echo "Building tools..."
	$(CC) $(CFLAGS) -c tools/perf_monitor.c -o $(OUTPUT_DIR)/perf_monitor.o

enterprise:
	@echo "Building enterprise features..."
	$(CC) $(CFLAGS) -c services/cluster_manager.c -o $(OUTPUT_DIR)/cluster_manager.o
	$(CC) $(CFLAGS) -c services/load_balancer.c -o $(OUTPUT_DIR)/load_balancer.o
	$(CC) $(CFLAGS) -c services/dist_cache.c -o $(OUTPUT_DIR)/dist_cache.o
	$(CC) $(CFLAGS) -c services/fault_tolerance.c -o $(OUTPUT_DIR)/fault_tolerance.o

advanced:
	@echo "Building advanced systems..."
	$(CC) $(CFLAGS) -c services/ml_inference.c -o $(OUTPUT_DIR)/ml_inference.o
	$(CC) $(CFLAGS) -c services/stream_processor.c -o $(OUTPUT_DIR)/stream_processor.o
	$(CC) $(CFLAGS) -c services/msg_broker.c -o $(OUTPUT_DIR)/msg_broker.o

observability:
	@echo "Building observability and security..."
	$(CC) $(CFLAGS) -c services/distributed_tracer.c -o $(OUTPUT_DIR)/distributed_tracer.o
	$(CC) $(CFLAGS) -c services/anomaly_detector.c -o $(OUTPUT_DIR)/anomaly_detector.o
	$(CC) $(CFLAGS) -c services/dist_kv_store.c -o $(OUTPUT_DIR)/dist_kv_store.o
	$(CC) $(CFLAGS) -c services/edge_runtime.c -o $(OUTPUT_DIR)/edge_runtime.o

part8:
	@echo "Building Part 8 - Advanced Enterprise Systems..."
	$(CC) $(CFLAGS) -c services/blockchain_audit.c -o $(OUTPUT_DIR)/blockchain_audit.o
	$(CC) $(CFLAGS) -c services/analytics_engine.c -o $(OUTPUT_DIR)/analytics_engine.o
	$(CC) $(CFLAGS) -c services/api_gateway.c -o $(OUTPUT_DIR)/api_gateway.o
	$(CC) $(CFLAGS) -c services/disaster_recovery.c -o $(OUTPUT_DIR)/disaster_recovery.o

part9:
	@echo "Building Part 9 - Advanced Security Systems..."
	$(CC) $(CFLAGS) -c services/threat_detector.c -o $(OUTPUT_DIR)/threat_detector.o
	$(CC) $(CFLAGS) -c services/data_loss_prevention.c -o $(OUTPUT_DIR)/data_loss_prevention.o
	$(CC) $(CFLAGS) -c services/intrusion_detection.c -o $(OUTPUT_DIR)/intrusion_detection.o
	$(CC) $(CFLAGS) -c services/security_policy_engine.c -o $(OUTPUT_DIR)/security_policy_engine.o

part10:
	@echo "Building Part 10 - System Optimization and Hardening..."
	$(CC) $(CFLAGS) -c services/performance_optimizer.c -o $(OUTPUT_DIR)/performance_optimizer.o
	$(CC) $(CFLAGS) -c services/system_hardening.c -o $(OUTPUT_DIR)/system_hardening.o
	$(CC) $(CFLAGS) -c services/resource_allocator.c -o $(OUTPUT_DIR)/resource_allocator.o -lm
	$(CC) $(CFLAGS) -c services/compliance_auditor.c -o $(OUTPUT_DIR)/compliance_auditor.o

part11:
	@echo "Building Part 11 - Advanced Scheduling and Workload Management..."
	$(CC) $(CFLAGS) -c services/advanced_scheduler.c -o $(OUTPUT_DIR)/advanced_scheduler_part11.o
	$(CC) $(CFLAGS) -c services/cost_optimizer.c -o $(OUTPUT_DIR)/cost_optimizer.o
	$(CC) $(CFLAGS) -c services/workload_manager.c -o $(OUTPUT_DIR)/workload_manager.o
	$(CC) $(CFLAGS) -c services/sla_manager.c -o $(OUTPUT_DIR)/sla_manager.o

part12:
	@echo "Building Part 12 - AI-Driven System Optimization..."
	$(CC) $(CFLAGS) -c services/predictive_optimizer.c -o $(OUTPUT_DIR)/predictive_optimizer.o -lm
	$(CC) $(CFLAGS) -c services/autonomous_resource_manager.c -o $(OUTPUT_DIR)/autonomous_resource_manager.o
	$(CC) $(CFLAGS) -c services/self_healing_manager.c -o $(OUTPUT_DIR)/self_healing_manager.o
	$(CC) $(CFLAGS) -c services/intelligence_coordinator.c -o $(OUTPUT_DIR)/intelligence_coordinator.o

oneos: $(OUTPUT_DIR)/ui_core.o $(OUTPUT_DIR)/menu_bar.o $(OUTPUT_DIR)/dock.o $(OUTPUT_DIR)/window_manager.o $(OUTPUT_DIR)/display_server.o $(OUTPUT_DIR)/input_manager.o $(OUTPUT_DIR)/graphics_core.o $(OUTPUT_DIR)/audio_engine.o $(OUTPUT_DIR)/process_scheduler.o $(OUTPUT_DIR)/ipc_manager.o $(OUTPUT_DIR)/thread_manager.o $(OUTPUT_DIR)/system_logger.o $(OUTPUT_DIR)/advanced_scheduler.o $(OUTPUT_DIR)/memory_manager.o $(OUTPUT_DIR)/virtual_memory.o $(OUTPUT_DIR)/memory_optimization.o $(OUTPUT_DIR)/filesystem_core.o $(OUTPUT_DIR)/advanced_filesystem.o $(OUTPUT_DIR)/fs_journal.o $(OUTPUT_DIR)/networking_core.o $(OUTPUT_DIR)/network_stack.o $(OUTPUT_DIR)/security_manager.o $(OUTPUT_DIR)/security_core.o $(OUTPUT_DIR)/compatibility_layer.o $(OUTPUT_DIR)/application_manager.o $(OUTPUT_DIR)/package_manager.o $(OUTPUT_DIR)/boot_manager.o $(OUTPUT_DIR)/power_manager.o $(OUTPUT_DIR)/virtualization.o $(OUTPUT_DIR)/container_manager.o $(OUTPUT_DIR)/perf_monitor.o $(OUTPUT_DIR)/cluster_manager.o $(OUTPUT_DIR)/load_balancer.o $(OUTPUT_DIR)/dist_cache.o $(OUTPUT_DIR)/fault_tolerance.o $(OUTPUT_DIR)/ml_inference.o $(OUTPUT_DIR)/stream_processor.o $(OUTPUT_DIR)/msg_broker.o $(OUTPUT_DIR)/distributed_tracer.o $(OUTPUT_DIR)/anomaly_detector.o $(OUTPUT_DIR)/dist_kv_store.o $(OUTPUT_DIR)/edge_runtime.o $(OUTPUT_DIR)/blockchain_audit.o $(OUTPUT_DIR)/analytics_engine.o $(OUTPUT_DIR)/api_gateway.o $(OUTPUT_DIR)/disaster_recovery.o $(OUTPUT_DIR)/threat_detector.o $(OUTPUT_DIR)/data_loss_prevention.o $(OUTPUT_DIR)/intrusion_detection.o $(OUTPUT_DIR)/security_policy_engine.o $(OUTPUT_DIR)/performance_optimizer.o $(OUTPUT_DIR)/system_hardening.o $(OUTPUT_DIR)/resource_allocator.o $(OUTPUT_DIR)/compliance_auditor.o $(OUTPUT_DIR)/advanced_scheduler_part11.o $(OUTPUT_DIR)/cost_optimizer.o $(OUTPUT_DIR)/workload_manager.o $(OUTPUT_DIR)/sla_manager.o $(OUTPUT_DIR)/predictive_optimizer.o $(OUTPUT_DIR)/autonomous_resource_manager.o $(OUTPUT_DIR)/self_healing_manager.o $(OUTPUT_DIR)/intelligence_coordinator.o $(OUTPUT_DIR)/kernel_hybrid.o $(OUTPUT_DIR)/interrupt_handler.o $(OUTPUT_DIR)/syscall_handler.o $(OUTPUT_DIR)/realtime.o $(KERNEL_OBJS)
	@echo "Linking OneOS..."
	$(CC) $(CFLAGS) -c oneos_main.c -o $(OUTPUT_DIR)/oneos_main.o
	$(CC) $(CFLAGS) -c oneos.c -o $(OUTPUT_DIR)/oneos.o
	$(CC) -o $(OUTPUT_DIR)/oneos $(OUTPUT_DIR)/*.o $(LDFLAGS) -lm
	@echo "OneOS build complete: $(OUTPUT_DIR)/oneos"

build:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OUTPUT_DIR)

clean:
	$(MAKE) -C $(KERNEL_DIR) clean
	$(MAKE) -C $(DRIVERS_DIR) clean
	$(MAKE) -C $(BIN_DIR) clean
	$(MAKE) -C $(FS_DIR) clean
	$(MAKE) -C $(ARCH_DIR) clean
	rm -rf $(OUTPUT_DIR)/*.o

distclean: clean
	@echo "Cleaning all build artifacts..."
	rm -rf $(BUILD_DIR)

help:
	@echo "========================================="
	@echo "OneOS Build System"
	@echo "========================================="
	@echo "Targets:"
	@echo "  all          - Build all components"
	@echo "  kernel       - Build kernel"
	@echo "  drivers      - Build drivers"
	@echo "  bin          - Build binary utilities"
	@echo "  fs           - Build filesystem"
	@echo "  arch         - Build architecture modules"
	@echo "  ui           - Build UI system"
	@echo "  core-services- Build core services"
	@echo "  mm           - Build memory manager"
	@echo "  net          - Build networking"
	@echo "  security     - Build security manager"
	@echo "  compat       - Build compatibility layer"
	@echo "  apps         - Build application manager"
	@echo "  packages     - Build package manager"
	@echo "  boot         - Build boot manager"
	@echo "  services     - Build system services"
	@echo "  oneos        - Link final OneOS executable"
	@echo "  clean        - Clean build artifacts"
	@echo "  distclean    - Clean everything"
	@echo "========================================="
