#ifndef ALERT_QUEUE_H
#define ALERT_QUEUE_H

#include <stdint.h>

typedef struct
{
    uint32_t queue_id;
    uint32_t queue_size;
    uint32_t current_items;
    uint32_t max_priority_level;
} alert_queue_config_t;

typedef struct
{
    uint32_t total_queued;
    uint32_t processed;
    uint32_t dropped;
    uint64_t avg_queue_time;
} alert_queue_stats_t;

int alert_queue_init(alert_queue_config_t *config);
int alert_queue_enqueue(uint32_t alert_id, uint32_t priority);
int alert_queue_dequeue(uint32_t *alert_id);
int alert_queue_peek(uint32_t *alert_id);
int alert_queue_get_size(uint32_t *size);
int alert_queue_clear(void);
int alert_queue_set_max_size(uint32_t new_size);
int alert_queue_get_stats(alert_queue_stats_t *stats);

#endif
