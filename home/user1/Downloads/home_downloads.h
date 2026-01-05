#ifndef HOME_DOWNLOADS_H
#define HOME_DOWNLOADS_H

#include <stdint.h>

typedef struct
{
    uint32_t download_id;
    const char *filename;
    uint64_t file_size;
    uint32_t download_status;
    uint32_t progress_percent;
} home_downloads_file_t;

typedef struct
{
    uint32_t active_downloads;
    uint64_t total_downloaded;
    uint32_t completed_count;
} home_downloads_state_t;

int home_downloads_init(const char *downloads_dir);
int home_downloads_start_download(const char *url, const char *filename);
int home_downloads_pause_download(uint32_t download_id);
int home_downloads_resume_download(uint32_t download_id);
int home_downloads_cancel_download(uint32_t download_id);
int home_downloads_list_downloads(home_downloads_file_t *downloads, uint32_t max_count);

#endif
