#ifndef BOOT_MANAGER_H
#define BOOT_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    BOOT_STAGE_FIRMWARE,
    BOOT_STAGE_BOOTLOADER,
    BOOT_STAGE_KERNEL,
    BOOT_STAGE_INITRD,
    BOOT_STAGE_FILESYSTEM,
    BOOT_STAGE_SERVICES,
    BOOT_STAGE_UI,
    BOOT_STAGE_LOGIN,
    BOOT_STAGE_DESKTOP,
    BOOT_STAGE_COMPLETE
} BootStage;

typedef struct
{
    BootStage stage;
    uint32_t progress_percent;
    char status_message[256];
    uint32_t boot_time_ms;
    bool is_verbose;
} BootProgress;

typedef struct
{
    BootProgress progress;
    bool secure_boot_enabled;
    bool uefi_mode;
    char boot_device[256];
    char kernel_params[512];
    uint32_t boot_count;
} BootManager;

BootManager *boot_manager_init(void);
void boot_manager_cleanup(BootManager *bm);
int boot_manager_load_firmware(BootManager *bm);
int boot_manager_load_kernel(BootManager *bm);
int boot_manager_mount_filesystems(BootManager *bm);
int boot_manager_start_services(BootManager *bm);
int boot_manager_start_ui(BootManager *bm);
void boot_manager_set_progress(BootManager *bm, BootStage stage, uint32_t progress);
void boot_manager_set_kernel_params(BootManager *bm, const char *params);
BootProgress *boot_manager_get_progress(BootManager *bm);

#endif
