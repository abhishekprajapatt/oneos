#include "thread_manager.h"
#include <stdlib.h>
#include <string.h>

ThreadManager *thread_init(void)
{
    ThreadManager *manager = (ThreadManager *)malloc(sizeof(ThreadManager));
    if (!manager)
        return NULL;

    memset(manager->threads, 0, MAX_THREADS * sizeof(Thread));
    memset(manager->mutexes, 0, MAX_MUTEXES * sizeof(Mutex));
    memset(manager->semaphores, 0, MAX_SEMAPHORES * sizeof(Semaphore));

    manager->thread_count = 0;
    manager->mutex_count = 0;
    manager->semaphore_count = 0;
    manager->current_thread_id = 0;
    manager->total_threads_created = 0;
    manager->context_switches = 0;

    return manager;
}

void thread_cleanup(ThreadManager *manager)
{
    if (!manager)
        return;

    for (uint32_t i = 0; i < manager->thread_count; i++)
    {
        if (manager->threads[i].stack)
        {
            free(manager->threads[i].stack);
        }
    }

    free(manager);
}

uint32_t thread_create(ThreadManager *manager, uint32_t parent_pid, thread_func_t start_routine,
                       void *arg, ThreadPriority priority, uint32_t stack_size, const char *name)
{
    if (!manager || manager->thread_count >= MAX_THREADS)
        return 0;
    if (!start_routine || stack_size == 0)
        return 0;

    Thread *thread = &manager->threads[manager->thread_count];
    thread->thread_id = manager->thread_count + 1;
    thread->parent_pid = parent_pid;
    thread->state = THREAD_CREATED;
    thread->priority = priority;
    thread->start_routine = start_routine;
    thread->arg = arg;
    thread->stack_size = stack_size;
    thread->stack = (uint8_t *)malloc(stack_size);
    if (!thread->stack)
        return 0;

    thread->stack_pointer = 0;
    thread->cpu_time_us = 0;
    thread->created_time = time(NULL);
    thread->terminated_time = 0;

    if (name)
    {
        strncpy(thread->name, name, MAX_THREAD_NAME_LEN - 1);
    }
    else
    {
        snprintf(thread->name, MAX_THREAD_NAME_LEN, "Thread-%u", thread->thread_id);
    }

    manager->thread_count++;
    manager->total_threads_created++;

    return thread->thread_id;
}

int thread_start(ThreadManager *manager, uint32_t thread_id)
{
    if (!manager || thread_id == 0 || thread_id > manager->thread_count)
        return -1;

    Thread *thread = &manager->threads[thread_id - 1];
    if (thread->state != THREAD_CREATED)
        return -1;

    thread->state = THREAD_RUNNING;
    return 0;
}

int thread_join(ThreadManager *manager, uint32_t thread_id)
{
    if (!manager || thread_id == 0 || thread_id > manager->thread_count)
        return -1;

    Thread *thread = &manager->threads[thread_id - 1];
    if (thread->state == THREAD_TERMINATED)
        return 0;

    thread->state = THREAD_WAITING;
    return 0;
}

int thread_detach(ThreadManager *manager, uint32_t thread_id)
{
    if (!manager || thread_id == 0 || thread_id > manager->thread_count)
        return -1;

    Thread *thread = &manager->threads[thread_id - 1];
    thread->state = THREAD_RUNNING;

    return 0;
}

int thread_exit(ThreadManager *manager, uint32_t thread_id)
{
    if (!manager || thread_id == 0 || thread_id > manager->thread_count)
        return -1;

    Thread *thread = &manager->threads[thread_id - 1];
    thread->state = THREAD_TERMINATED;
    thread->terminated_time = time(NULL);

    return 0;
}

int thread_cancel(ThreadManager *manager, uint32_t thread_id)
{
    if (!manager || thread_id == 0 || thread_id > manager->thread_count)
        return -1;

    Thread *thread = &manager->threads[thread_id - 1];
    if (thread->state == THREAD_TERMINATED)
        return -1;

    thread->state = THREAD_TERMINATED;
    thread->terminated_time = time(NULL);

    return 0;
}

int thread_suspend(ThreadManager *manager, uint32_t thread_id)
{
    if (!manager || thread_id == 0 || thread_id > manager->thread_count)
        return -1;

    Thread *thread = &manager->threads[thread_id - 1];
    if (thread->state == THREAD_TERMINATED)
        return -1;

    thread->state = THREAD_SUSPENDED;
    return 0;
}

int thread_resume(ThreadManager *manager, uint32_t thread_id)
{
    if (!manager || thread_id == 0 || thread_id > manager->thread_count)
        return -1;

    Thread *thread = &manager->threads[thread_id - 1];
    if (thread->state != THREAD_SUSPENDED)
        return -1;

    thread->state = THREAD_RUNNING;
    return 0;
}

ThreadState thread_get_state(ThreadManager *manager, uint32_t thread_id)
{
    if (!manager || thread_id == 0 || thread_id > manager->thread_count)
        return THREAD_TERMINATED;
    return manager->threads[thread_id - 1].state;
}

uint32_t mutex_create(ThreadManager *manager)
{
    if (!manager || manager->mutex_count >= MAX_MUTEXES)
        return 0;

    Mutex *mutex = &manager->mutexes[manager->mutex_count];
    mutex->mutex_id = manager->mutex_count + 1;
    mutex->owner_thread_id = 0;
    mutex->is_locked = false;
    mutex->lock_count = 0;
    mutex->waiting_count = 0;

    manager->mutex_count++;
    return mutex->mutex_id;
}

int mutex_lock(ThreadManager *manager, uint32_t mutex_id, uint32_t thread_id)
{
    if (!manager || mutex_id == 0 || mutex_id > manager->mutex_count)
        return -1;

    Mutex *mutex = &manager->mutexes[mutex_id - 1];

    if (mutex->owner_thread_id == thread_id)
    {
        mutex->lock_count++;
        return 0;
    }

    if (mutex->is_locked)
    {
        if (mutex->waiting_count < MAX_THREADS)
        {
            mutex->waiting_threads[mutex->waiting_count++] = thread_id;
        }
        return -1;
    }

    mutex->is_locked = true;
    mutex->owner_thread_id = thread_id;
    mutex->lock_count = 1;

    return 0;
}

int mutex_unlock(ThreadManager *manager, uint32_t mutex_id, uint32_t thread_id)
{
    if (!manager || mutex_id == 0 || mutex_id > manager->mutex_count)
        return -1;

    Mutex *mutex = &manager->mutexes[mutex_id - 1];
    if (mutex->owner_thread_id != thread_id || !mutex->is_locked)
        return -1;

    mutex->lock_count--;
    if (mutex->lock_count == 0)
    {
        mutex->is_locked = false;
        mutex->owner_thread_id = 0;

        if (mutex->waiting_count > 0)
        {
            memmove(&mutex->waiting_threads[0], &mutex->waiting_threads[1],
                    (mutex->waiting_count - 1) * sizeof(uint32_t));
            mutex->waiting_count--;
        }
    }

    return 0;
}

int mutex_trylock(ThreadManager *manager, uint32_t mutex_id, uint32_t thread_id)
{
    if (!manager || mutex_id == 0 || mutex_id > manager->mutex_count)
        return -1;

    Mutex *mutex = &manager->mutexes[mutex_id - 1];

    if (mutex->is_locked && mutex->owner_thread_id != thread_id)
    {
        return -1;
    }

    if (mutex->owner_thread_id == thread_id)
    {
        mutex->lock_count++;
        return 0;
    }

    mutex->is_locked = true;
    mutex->owner_thread_id = thread_id;
    mutex->lock_count = 1;

    return 0;
}

int mutex_destroy(ThreadManager *manager, uint32_t mutex_id)
{
    if (!manager || mutex_id == 0 || mutex_id > manager->mutex_count)
        return -1;

    Mutex *mutex = &manager->mutexes[mutex_id - 1];
    if (mutex->is_locked || mutex->waiting_count > 0)
        return -1;

    mutex->mutex_id = 0;
    return 0;
}

uint32_t semaphore_create(ThreadManager *manager, uint32_t initial_count, uint32_t max_count)
{
    if (!manager || manager->semaphore_count >= MAX_SEMAPHORES)
        return 0;

    Semaphore *sem = &manager->semaphores[manager->semaphore_count];
    sem->semaphore_id = manager->semaphore_count + 1;
    sem->count = initial_count;
    sem->max_count = max_count;
    sem->waiting_count = 0;

    manager->semaphore_count++;
    return sem->semaphore_id;
}

int semaphore_wait(ThreadManager *manager, uint32_t sem_id, uint32_t thread_id)
{
    if (!manager || sem_id == 0 || sem_id > manager->semaphore_count)
        return -1;

    Semaphore *sem = &manager->semaphores[sem_id - 1];

    if (sem->count > 0)
    {
        sem->count--;
        return 0;
    }

    if (sem->waiting_count < MAX_THREADS)
    {
        sem->waiting_threads[sem->waiting_count++] = thread_id;
    }

    return -1;
}

int semaphore_post(ThreadManager *manager, uint32_t sem_id)
{
    if (!manager || sem_id == 0 || sem_id > manager->semaphore_count)
        return -1;

    Semaphore *sem = &manager->semaphores[sem_id - 1];

    if (sem->count >= sem->max_count)
        return -1;

    sem->count++;

    if (sem->waiting_count > 0)
    {
        memmove(&sem->waiting_threads[0], &sem->waiting_threads[1],
                (sem->waiting_count - 1) * sizeof(uint32_t));
        sem->waiting_count--;
    }

    return 0;
}

int semaphore_getvalue(ThreadManager *manager, uint32_t sem_id)
{
    if (!manager || sem_id == 0 || sem_id > manager->semaphore_count)
        return -1;
    return manager->semaphores[sem_id - 1].count;
}

int semaphore_destroy(ThreadManager *manager, uint32_t sem_id)
{
    if (!manager || sem_id == 0 || sem_id > manager->semaphore_count)
        return -1;

    Semaphore *sem = &manager->semaphores[sem_id - 1];
    if (sem->waiting_count > 0)
        return -1;

    sem->semaphore_id = 0;
    return 0;
}

uint32_t thread_get_count(ThreadManager *manager)
{
    if (!manager)
        return 0;
    return manager->thread_count;
}

uint32_t thread_get_current_id(ThreadManager *manager)
{
    if (!manager)
        return 0;
    return manager->current_thread_id;
}

uint64_t thread_get_total_created(ThreadManager *manager)
{
    if (!manager)
        return 0;
    return manager->total_threads_created;
}
