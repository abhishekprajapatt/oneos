#include "msg_broker.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static MessageBroker g_brokers[8] = {0};
static uint32_t g_broker_count = 0;

int mb_create_broker(const char *name)
{
    if (!name || g_broker_count >= 8)
    {
        return -1;
    }

    MessageBroker *broker = &g_brokers[g_broker_count];
    broker->broker_id = g_broker_count + 1;
    strncpy(broker->broker_name, name, 63);
    broker->queue_count = 0;
    broker->subscriber_count = 0;
    broker->total_messages_routed = 0;
    broker->total_bytes_routed = 0;
    broker->failed_deliveries = 0;
    broker->dlq_size = 0;

    broker->queues = malloc(MAX_MESSAGE_QUEUES * sizeof(MessageQueue));
    if (!broker->queues)
    {
        return -1;
    }

    g_broker_count++;
    return broker->broker_id;
}

int mb_delete_broker(uint32_t broker_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return -1;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];

    for (uint32_t i = 0; i < broker->queue_count; i++)
    {
        if (broker->queues[i].messages)
        {
            for (uint32_t j = 0; j < broker->queues[i].message_count; j++)
            {
                if (broker->queues[i].messages[j].payload)
                {
                    free(broker->queues[i].messages[j].payload);
                }
            }
            free(broker->queues[i].messages);
        }
    }

    if (broker->queues)
    {
        free(broker->queues);
    }

    for (uint32_t i = broker_id - 1; i < g_broker_count - 1; i++)
    {
        g_brokers[i] = g_brokers[i + 1];
    }
    g_broker_count--;

    return 0;
}

int mb_create_queue(uint32_t broker_id, const char *queue_name, QueueType queue_type, uint32_t capacity)
{
    if (broker_id == 0 || broker_id > g_broker_count || !queue_name)
    {
        return -1;
    }

    if (capacity > MAX_QUEUE_SIZE || capacity == 0)
    {
        return -1;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];
    if (broker->queue_count >= MAX_MESSAGE_QUEUES)
    {
        return -1;
    }

    MessageQueue *queue = &broker->queues[broker->queue_count];
    queue->queue_id = broker->queue_count + 1;
    strncpy(queue->queue_name, queue_name, 63);
    queue->queue_type = queue_type;
    queue->delivery_guarantee = DELIVERY_AT_LEAST_ONCE;
    queue->message_count = 0;
    queue->capacity = capacity;
    queue->head = 0;
    queue->tail = 0;
    queue->total_messages = 0;
    queue->total_bytes = 0;
    queue->subscriber_count = 0;
    queue->last_activity = time(NULL);

    queue->messages = malloc(capacity * sizeof(Message));
    if (!queue->messages)
    {
        return -1;
    }

    broker->queue_count++;
    return queue->queue_id;
}

int mb_delete_queue(uint32_t broker_id, uint32_t queue_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return -1;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];
    if (queue_id == 0 || queue_id > broker->queue_count)
    {
        return -1;
    }

    MessageQueue *queue = &broker->queues[queue_id - 1];
    if (queue->messages)
    {
        free(queue->messages);
    }

    for (uint32_t i = queue_id - 1; i < broker->queue_count - 1; i++)
    {
        broker->queues[i] = broker->queues[i + 1];
    }
    broker->queue_count--;

    return 0;
}

int mb_set_delivery_guarantee(uint32_t broker_id, uint32_t queue_id, DeliveryGuarantee guarantee)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return -1;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];
    if (queue_id == 0 || queue_id > broker->queue_count)
    {
        return -1;
    }

    broker->queues[queue_id - 1].delivery_guarantee = guarantee;
    return 0;
}

int mb_set_queue_ttl(uint32_t broker_id, uint32_t queue_id, uint32_t ttl_seconds)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return -1;
    }

    return 0;
}

int mb_purge_queue(uint32_t broker_id, uint32_t queue_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return -1;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];
    if (queue_id == 0 || queue_id > broker->queue_count)
    {
        return -1;
    }

    MessageQueue *queue = &broker->queues[queue_id - 1];
    queue->message_count = 0;
    queue->head = 0;
    queue->tail = 0;

    return 0;
}

int mb_send_message(uint32_t broker_id, uint32_t queue_id, const void *payload, uint32_t size)
{
    if (broker_id == 0 || broker_id > g_broker_count || !payload)
    {
        return -1;
    }

    if (size > MAX_MESSAGE_SIZE)
    {
        return -1;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];
    if (queue_id == 0 || queue_id > broker->queue_count)
    {
        return -1;
    }

    MessageQueue *queue = &broker->queues[queue_id - 1];
    if (queue->message_count >= queue->capacity)
    {
        return -1;
    }

    Message *msg = &queue->messages[queue->tail];
    msg->message_id = queue->total_messages + 1;
    msg->timestamp = time(NULL);
    msg->priority = 0;
    msg->payload = malloc(size);

    if (!msg->payload)
    {
        return -1;
    }

    memcpy(msg->payload, payload, size);
    msg->payload_size = size;
    msg->state = MSG_PENDING;
    msg->delivery_attempts = 0;
    strncpy(msg->sender_id, "producer", 63);
    msg->expiration_time = msg->timestamp + 86400;

    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->message_count++;
    queue->total_messages++;
    queue->total_bytes += size;
    queue->last_activity = time(NULL);

    broker->total_messages_routed++;
    broker->total_bytes_routed += size;

    return msg->message_id;
}

int mb_send_priority_message(uint32_t broker_id, uint32_t queue_id, const void *payload, uint32_t size, uint32_t priority)
{
    if (broker_id == 0 || broker_id > g_broker_count || !payload)
    {
        return -1;
    }

    int msg_id = mb_send_message(broker_id, queue_id, payload, size);

    if (msg_id > 0)
    {
        MessageBroker *broker = &g_brokers[broker_id - 1];
        MessageQueue *queue = &broker->queues[queue_id - 1];

        uint32_t insert_idx = (queue->tail - 1 + queue->capacity) % queue->capacity;
        queue->messages[insert_idx].priority = priority;
    }

    return msg_id;
}

int mb_receive_message(uint32_t broker_id, uint32_t queue_id, void *payload, uint32_t *size)
{
    if (broker_id == 0 || broker_id > g_broker_count || !payload || !size)
    {
        return -1;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];
    if (queue_id == 0 || queue_id > broker->queue_count)
    {
        return -1;
    }

    MessageQueue *queue = &broker->queues[queue_id - 1];
    if (queue->message_count == 0)
    {
        *size = 0;
        return 0;
    }

    Message *msg = &queue->messages[queue->head];
    if (*size < msg->payload_size)
    {
        return -1;
    }

    memcpy(payload, msg->payload, msg->payload_size);
    *size = msg->payload_size;

    msg->state = MSG_DELIVERED;
    msg->delivery_attempts++;

    queue->head = (queue->head + 1) % queue->capacity;
    queue->message_count--;
    queue->last_activity = time(NULL);

    return msg->message_id;
}

int mb_peek_message(uint32_t broker_id, uint32_t queue_id, void *payload, uint32_t *size)
{
    if (broker_id == 0 || broker_id > g_broker_count || !payload || !size)
    {
        return -1;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];
    if (queue_id == 0 || queue_id > broker->queue_count)
    {
        return -1;
    }

    MessageQueue *queue = &broker->queues[queue_id - 1];
    if (queue->message_count == 0)
    {
        *size = 0;
        return 0;
    }

    Message *msg = &queue->messages[queue->head];
    if (*size < msg->payload_size)
    {
        return -1;
    }

    memcpy(payload, msg->payload, msg->payload_size);
    *size = msg->payload_size;

    return msg->message_id;
}

int mb_subscribe(uint32_t broker_id, uint32_t queue_id, const char *subscriber_name)
{
    if (broker_id == 0 || broker_id > g_broker_count || !subscriber_name)
    {
        return -1;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];
    if (queue_id == 0 || queue_id > broker->queue_count)
    {
        return -1;
    }

    if (broker->subscriber_count >= MAX_SUBSCRIBERS)
    {
        return -1;
    }

    Subscriber *sub = &broker->subscribers[broker->subscriber_count];
    sub->subscriber_id = broker->subscriber_count + 1;
    strncpy(sub->subscriber_name, subscriber_name, 63);
    sub->queue_id = queue_id;
    sub->messages_consumed = 0;
    sub->bytes_consumed = 0;
    sub->pending_acks = 0;
    sub->last_activity = time(NULL);
    sub->is_connected = 1;

    broker->queues[queue_id - 1].subscriber_count++;
    broker->subscriber_count++;

    return sub->subscriber_id;
}

int mb_unsubscribe(uint32_t broker_id, uint32_t subscriber_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return -1;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];
    if (subscriber_id == 0 || subscriber_id > broker->subscriber_count)
    {
        return -1;
    }

    for (uint32_t i = subscriber_id - 1; i < broker->subscriber_count - 1; i++)
    {
        broker->subscribers[i] = broker->subscribers[i + 1];
    }
    broker->subscriber_count--;

    return 0;
}

int mb_connect_subscriber(uint32_t broker_id, uint32_t subscriber_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return -1;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];
    if (subscriber_id == 0 || subscriber_id > broker->subscriber_count)
    {
        return -1;
    }

    broker->subscribers[subscriber_id - 1].is_connected = 1;
    broker->subscribers[subscriber_id - 1].last_activity = time(NULL);

    return 0;
}

int mb_disconnect_subscriber(uint32_t broker_id, uint32_t subscriber_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return -1;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];
    if (subscriber_id == 0 || subscriber_id > broker->subscriber_count)
    {
        return -1;
    }

    broker->subscribers[subscriber_id - 1].is_connected = 0;

    return 0;
}

int mb_acknowledge_message(uint32_t broker_id, uint32_t subscriber_id, uint64_t message_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return -1;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];
    if (subscriber_id == 0 || subscriber_id > broker->subscriber_count)
    {
        return -1;
    }

    Subscriber *sub = &broker->subscribers[subscriber_id - 1];
    if (sub->pending_acks > 0)
    {
        sub->pending_acks--;
    }

    return 0;
}

int mb_nack_message(uint32_t broker_id, uint32_t subscriber_id, uint64_t message_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return -1;
    }

    broker->failed_deliveries++;
    return 0;
}

int mb_move_to_dlq(uint32_t broker_id, uint32_t queue_id, uint64_t message_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return -1;
    }

    g_brokers[broker_id - 1].dlq_size++;
    return 0;
}

int mb_retry_message(uint32_t broker_id, uint32_t queue_id, uint64_t message_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return -1;
    }

    return 0;
}

int mb_get_dlq_message(uint32_t broker_id, void *payload, uint32_t *size)
{
    if (broker_id == 0 || broker_id > g_broker_count || !payload || !size)
    {
        return -1;
    }

    return 0;
}

int mb_requeue_dlq_message(uint32_t broker_id, uint32_t queue_id, uint64_t message_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return -1;
    }

    if (g_brokers[broker_id - 1].dlq_size > 0)
    {
        g_brokers[broker_id - 1].dlq_size--;
    }

    return 0;
}

MessageBroker *mb_get_broker_info(uint32_t broker_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return NULL;
    }

    return &g_brokers[broker_id - 1];
}

MessageQueue *mb_get_queue_info(uint32_t broker_id, uint32_t queue_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return NULL;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];
    if (queue_id == 0 || queue_id > broker->queue_count)
    {
        return NULL;
    }

    return &broker->queues[queue_id - 1];
}

uint32_t mb_get_queue_depth(uint32_t broker_id, uint32_t queue_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return 0;
    }

    MessageBroker *broker = &g_brokers[broker_id - 1];
    if (queue_id == 0 || queue_id > broker->queue_count)
    {
        return 0;
    }

    return broker->queues[queue_id - 1].message_count;
}

uint64_t mb_get_broker_throughput(uint32_t broker_id)
{
    if (broker_id == 0 || broker_id > g_broker_count)
    {
        return 0;
    }

    return g_brokers[broker_id - 1].total_messages_routed;
}
