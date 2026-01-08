#include "boot_manager.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

BootManager *boot_manager_init(void)
{
    BootManager *bm = (BootManager *)malloc(sizeof(BootManager));
    if (!bm)
        return NULL;

    bm->progress.stage = BOOT_STAGE_FIRMWARE;
    bm->progress.progress_percent = 0;
    bm->progress.boot_time_ms = 0;
    bm->progress.is_verbose = false;
    strcpy(bm->progress.status_message, "OneOS Boot Starting...");

    bm->secure_boot_enabled = true;
    bm->uefi_mode = true;
    strcpy(bm->boot_device, "/dev/sda1");
    strcpy(bm->kernel_params, "root=/dev/sda1 rw console=ttyS0");
    bm->boot_count = 0;

    return bm;
}

void boot_manager_cleanup(BootManager *bm)
{
    if (!bm)
        return;
    free(bm);
}

int boot_manager_load_firmware(BootManager *bm)
{
    if (!bm)
        return -1;

    bm->progress.stage = BOOT_STAGE_FIRMWARE;
    bm->progress.progress_percent = 10;
    strcpy(bm->progress.status_message, "Loading firmware...");

    return 0;
}

int boot_manager_load_kernel(BootManager *bm)
{
    if (!bm)
        return -1;

    bm->progress.stage = BOOT_STAGE_KERNEL;
    bm->progress.progress_percent = 25;
    strcpy(bm->progress.status_message, "Loading kernel...");

    return 0;
}

int boot_manager_mount_filesystems(BootManager *bm)
{
    if (!bm)
        return -1;

    bm->progress.stage = BOOT_STAGE_FILESYSTEM;
    bm->progress.progress_percent = 40;
    strcpy(bm->progress.status_message, "Mounting filesystems...");

    return 0;
}

int boot_manager_start_services(BootManager *bm)
{
    if (!bm)
        return -1;

    bm->progress.stage = BOOT_STAGE_SERVICES;
    bm->progress.progress_percent = 60;
    strcpy(bm->progress.status_message, "Starting core services...");

    return 0;
}

int boot_manager_start_ui(BootManager *bm)
{
    if (!bm)
        return -1;

    bm->progress.stage = BOOT_STAGE_UI;
    bm->progress.progress_percent = 80;
    strcpy(bm->progress.status_message, "Starting UI system...");

    return 0;
}

void boot_manager_set_progress(BootManager *bm, BootStage stage, uint32_t progress)
{
    if (!bm)
        return;

    bm->progress.stage = stage;
    bm->progress.progress_percent = progress;
}

void boot_manager_set_kernel_params(BootManager *bm, const char *params)
{
    if (!bm || !params)
        return;

    strncpy(bm->kernel_params, params, 511);
}

BootProgress *boot_manager_get_progress(BootManager *bm)
{
    if (!bm)
        return NULL;
    return &bm->progress;
}
