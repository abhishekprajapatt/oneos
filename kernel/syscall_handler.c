#include "syscall_handler.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

SyscallHandler *syscall_init(void)
{
    SyscallHandler *handler = (SyscallHandler *)malloc(sizeof(SyscallHandler));
    if (!handler)
        return NULL;

    memset(handler->descriptors, 0, MAX_SYSCALLS * sizeof(SyscallDescriptor));
    handler->handler_count = 0;
    handler->total_syscalls = 0;

    return handler;
}

void syscall_cleanup(SyscallHandler *handler)
{
    if (!handler)
        return;
    free(handler);
}

int syscall_register(SyscallHandler *handler, SyscallNumber num, syscall_handler_t func, const char *name)
{
    if (!handler || !func || num >= MAX_SYSCALLS)
        return -1;
    if (handler->handler_count >= MAX_SYSCALLS)
        return -1;

    SyscallDescriptor *desc = &handler->descriptors[num];
    desc->syscall_num = num;
    desc->handler = func;
    desc->name = name;
    desc->call_count = 0;
    desc->total_time_us = 0;

    handler->handler_count++;
    return 0;
}

int64_t syscall_invoke(SyscallHandler *handler, SyscallNumber num, SyscallContext *ctx)
{
    if (!handler || !ctx || num >= MAX_SYSCALLS)
        return -1;

    SyscallDescriptor *desc = &handler->descriptors[num];
    if (!desc->handler)
        return -1;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int64_t result = desc->handler(ctx);

    clock_gettime(CLOCK_MONOTONIC, &end);
    uint64_t elapsed_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;

    desc->call_count++;
    desc->total_time_us += elapsed_us;
    handler->total_syscalls++;
    ctx->return_value = result;

    return result;
}

uint32_t syscall_get_handler_count(SyscallHandler *handler)
{
    if (!handler)
        return 0;
    return handler->handler_count;
}

uint64_t syscall_get_total_calls(SyscallHandler *handler)
{
    if (!handler)
        return 0;
    return handler->total_syscalls;
}

int64_t syscall_read(SyscallContext *ctx)
{
    if (!ctx)
        return -1;
    uint32_t fd = (uint32_t)ctx->args[0];
    char *buffer = (char *)ctx->args[1];
    uint32_t count = (uint32_t)ctx->args[2];

    if (!buffer || count == 0)
        return -1;

    memset(buffer, 0, count);
    return count;
}

int64_t syscall_write(SyscallContext *ctx)
{
    if (!ctx)
        return -1;
    uint32_t fd = (uint32_t)ctx->args[0];
    const char *buffer = (const char *)ctx->args[1];
    uint32_t count = (uint32_t)ctx->args[2];

    if (!buffer || count == 0)
        return -1;

    return count;
}

int64_t syscall_open(SyscallContext *ctx)
{
    if (!ctx)
        return -1;
    const char *path = (const char *)ctx->args[0];
    uint32_t flags = (uint32_t)ctx->args[1];
    uint32_t mode = (uint32_t)ctx->args[2];

    if (!path)
        return -1;

    return 3;
}

int64_t syscall_close(SyscallContext *ctx)
{
    if (!ctx)
        return -1;
    uint32_t fd = (uint32_t)ctx->args[0];

    if (fd < 0)
        return -1;

    return 0;
}

int64_t syscall_fork(SyscallContext *ctx)
{
    if (!ctx)
        return -1;

    return 1001;
}

int64_t syscall_exit(SyscallContext *ctx)
{
    if (!ctx)
        return -1;
    uint32_t exit_code = (uint32_t)ctx->args[0];

    return 0;
}

int64_t syscall_exec(SyscallContext *ctx)
{
    if (!ctx)
        return -1;
    const char *path = (const char *)ctx->args[0];
    char **argv = (char **)ctx->args[1];

    if (!path || !argv)
        return -1;

    return 0;
}

int64_t syscall_wait(SyscallContext *ctx)
{
    if (!ctx)
        return -1;
    uint32_t pid = (uint32_t)ctx->args[0];

    if (pid == 0)
        return -1;

    return pid;
}

int64_t syscall_getpid(SyscallContext *ctx)
{
    if (!ctx)
        return -1;

    return 1000;
}

int64_t syscall_kill(SyscallContext *ctx)
{
    if (!ctx)
        return -1;
    uint32_t pid = (uint32_t)ctx->args[0];
    uint32_t signal = (uint32_t)ctx->args[1];

    if (pid == 0 || signal == 0)
        return -1;

    return 0;
}
