#ifndef TOAST_QUEUE_H
#define TOAST_QUEUE_H

#include <stdint.h>

typedef struct
{
    uint32_t queue_id;
    uint32_t queue_size;
    uint32_t current_items;
    uint32_t allow_stacking;
} toast_queue_config_t;

typedef struct
{
    uint32_t total_queued;
    uint32_t displayed;
    uint32_t dismissed;
    uint64_t avg_display_time;
} toast_queue_stats_t;

int toast_queue_init(toast_queue_config_t *config);
int toast_queue_enqueue(uint32_t toast_id);
int toast_queue_dequeue(uint32_t *toast_id);
int toast_queue_peek(uint32_t *toast_id);
int toast_queue_get_size(uint32_t *size);
int toast_queue_clear(void);
int toast_queue_enable_stacking(uint32_t enable);
int toast_queue_get_stats(toast_queue_stats_t *stats);

#endif
