#ifndef MSG_BROKER_H
#define MSG_BROKER_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

#define MAX_MESSAGE_QUEUES 256
#define MAX_QUEUE_SIZE 32768
#define MAX_SUBSCRIBERS 512
#define MAX_MESSAGE_SIZE 262144

typedef enum
{
    QUEUE_FIFO = 0,
    QUEUE_PRIORITY = 1,
    QUEUE_LIFO = 2
} QueueType;

typedef enum
{
    DELIVERY_AT_MOST_ONCE = 0,
    DELIVERY_AT_LEAST_ONCE = 1,
    DELIVERY_EXACTLY_ONCE = 2
} DeliveryGuarantee;

typedef enum
{
    MSG_PENDING = 0,
    MSG_DELIVERED = 1,
    MSG_ACKNOWLEDGED = 2,
    MSG_DEAD_LETTER = 3
} MessageState;

typedef struct
{
    uint64_t message_id;
    uint64_t timestamp;
    uint32_t priority;
    void *payload;
    uint32_t payload_size;
    MessageState state;
    uint32_t delivery_attempts;
    char sender_id[64];
    time_t expiration_time;
} Message;

typedef struct
{
    uint32_t queue_id;
    char queue_name[64];
    QueueType queue_type;
    DeliveryGuarantee delivery_guarantee;
    uint32_t message_count;
    Message *messages;
    uint32_t capacity;
    uint32_t head;
    uint32_t tail;
    uint64_t total_messages;
    uint64_t total_bytes;
    uint32_t subscriber_count;
    uint64_t last_activity;
} MessageQueue;

typedef struct
{
    uint32_t subscriber_id;
    char subscriber_name[64];
    uint32_t queue_id;
    uint32_t messages_consumed;
    uint64_t bytes_consumed;
    uint32_t pending_acks;
    uint64_t last_activity;
    uint8_t is_connected;
} Subscriber;

typedef struct
{
    uint32_t broker_id;
    char broker_name[64];
    uint32_t queue_count;
    MessageQueue *queues;
    uint32_t subscriber_count;
    Subscriber subscribers[MAX_SUBSCRIBERS];
    uint64_t total_messages_routed;
    uint64_t total_bytes_routed;
    uint64_t failed_deliveries;
    uint32_t dlq_size;
} MessageBroker;

int mb_create_broker(const char *name);
int mb_delete_broker(uint32_t broker_id);

int mb_create_queue(uint32_t broker_id, const char *queue_name, QueueType queue_type, uint32_t capacity);
int mb_delete_queue(uint32_t broker_id, uint32_t queue_id);
int mb_set_delivery_guarantee(uint32_t broker_id, uint32_t queue_id, DeliveryGuarantee guarantee);
int mb_set_queue_ttl(uint32_t broker_id, uint32_t queue_id, uint32_t ttl_seconds);
int mb_purge_queue(uint32_t broker_id, uint32_t queue_id);

int mb_send_message(uint32_t broker_id, uint32_t queue_id, const void *payload, uint32_t size);
int mb_send_priority_message(uint32_t broker_id, uint32_t queue_id, const void *payload, uint32_t size, uint32_t priority);
int mb_receive_message(uint32_t broker_id, uint32_t queue_id, void *payload, uint32_t *size);
int mb_peek_message(uint32_t broker_id, uint32_t queue_id, void *payload, uint32_t *size);

int mb_subscribe(uint32_t broker_id, uint32_t queue_id, const char *subscriber_name);
int mb_unsubscribe(uint32_t broker_id, uint32_t subscriber_id);
int mb_connect_subscriber(uint32_t broker_id, uint32_t subscriber_id);
int mb_disconnect_subscriber(uint32_t broker_id, uint32_t subscriber_id);

int mb_acknowledge_message(uint32_t broker_id, uint32_t subscriber_id, uint64_t message_id);
int mb_nack_message(uint32_t broker_id, uint32_t subscriber_id, uint64_t message_id);
int mb_move_to_dlq(uint32_t broker_id, uint32_t queue_id, uint64_t message_id);
int mb_retry_message(uint32_t broker_id, uint32_t queue_id, uint64_t message_id);

int mb_get_dlq_message(uint32_t broker_id, void *payload, uint32_t *size);
int mb_requeue_dlq_message(uint32_t broker_id, uint32_t queue_id, uint64_t message_id);

MessageBroker *mb_get_broker_info(uint32_t broker_id);
MessageQueue *mb_get_queue_info(uint32_t broker_id, uint32_t queue_id);
uint32_t mb_get_queue_depth(uint32_t broker_id, uint32_t queue_id);
uint64_t mb_get_broker_throughput(uint32_t broker_id);

#endif 
