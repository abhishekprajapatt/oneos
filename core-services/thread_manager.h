#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_THREADS 4096
#define MAX_MUTEXES 2048
#define MAX_SEMAPHORES 2048
#define MAX_THREAD_NAME_LEN 64

typedef enum
{
    THREAD_CREATED = 0,
    THREAD_RUNNING = 1,
    THREAD_BLOCKED = 2,
    THREAD_WAITING = 3,
    THREAD_SUSPENDED = 4,
    THREAD_TERMINATED = 5
} ThreadState;

typedef enum
{
    PRIORITY_REALTIME = 0,
    PRIORITY_HIGH = 1,
    PRIORITY_NORMAL = 2,
    PRIORITY_LOW = 3,
    PRIORITY_IDLE = 4
} ThreadPriority;

typedef int (*thread_func_t)(void *arg);

typedef struct
{
    uint32_t thread_id;
    uint32_t parent_pid;
    ThreadState state;
    ThreadPriority priority;
    thread_func_t start_routine;
    void *arg;
    uint32_t stack_size;
    uint8_t *stack;
    uint32_t stack_pointer;
    uint64_t cpu_time_us;
    time_t created_time;
    time_t terminated_time;
    char name[MAX_THREAD_NAME_LEN];
} Thread;

typedef struct
{
    uint32_t mutex_id;
    uint32_t owner_thread_id;
    bool is_locked;
    uint32_t lock_count;
    uint32_t waiting_threads[MAX_THREADS];
    uint32_t waiting_count;
} Mutex;

typedef struct
{
    uint32_t semaphore_id;
    uint32_t count;
    uint32_t max_count;
    uint32_t waiting_threads[MAX_THREADS];
    uint32_t waiting_count;
} Semaphore;

typedef struct
{
    Thread threads[MAX_THREADS];
    uint32_t thread_count;
    Mutex mutexes[MAX_MUTEXES];
    uint32_t mutex_count;
    Semaphore semaphores[MAX_SEMAPHORES];
    uint32_t semaphore_count;
    uint32_t current_thread_id;
    uint64_t total_threads_created;
    uint64_t context_switches;
} ThreadManager;

ThreadManager *thread_init(void);
void thread_cleanup(ThreadManager *manager);

uint32_t thread_create(ThreadManager *manager, uint32_t parent_pid, thread_func_t start_routine,
                       void *arg, ThreadPriority priority, uint32_t stack_size, const char *name);
int thread_start(ThreadManager *manager, uint32_t thread_id);
int thread_join(ThreadManager *manager, uint32_t thread_id);
int thread_detach(ThreadManager *manager, uint32_t thread_id);
int thread_exit(ThreadManager *manager, uint32_t thread_id);
int thread_cancel(ThreadManager *manager, uint32_t thread_id);
int thread_suspend(ThreadManager *manager, uint32_t thread_id);
int thread_resume(ThreadManager *manager, uint32_t thread_id);
ThreadState thread_get_state(ThreadManager *manager, uint32_t thread_id);

uint32_t mutex_create(ThreadManager *manager);
int mutex_lock(ThreadManager *manager, uint32_t mutex_id, uint32_t thread_id);
int mutex_unlock(ThreadManager *manager, uint32_t mutex_id, uint32_t thread_id);
int mutex_trylock(ThreadManager *manager, uint32_t mutex_id, uint32_t thread_id);
int mutex_destroy(ThreadManager *manager, uint32_t mutex_id);

uint32_t semaphore_create(ThreadManager *manager, uint32_t initial_count, uint32_t max_count);
int semaphore_wait(ThreadManager *manager, uint32_t sem_id, uint32_t thread_id);
int semaphore_post(ThreadManager *manager, uint32_t sem_id);
int semaphore_getvalue(ThreadManager *manager, uint32_t sem_id);
int semaphore_destroy(ThreadManager *manager, uint32_t sem_id);

uint32_t thread_get_count(ThreadManager *manager);
uint32_t thread_get_current_id(ThreadManager *manager);
uint64_t thread_get_total_created(ThreadManager *manager);

#endif
