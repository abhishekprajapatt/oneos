#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_SYSCALLS 512
#define MAX_SYSCALL_ARGS 6
#define MAX_SYSCALL_HANDLERS 256

typedef enum
{
    SYSCALL_READ = 0,
    SYSCALL_WRITE = 1,
    SYSCALL_OPEN = 2,
    SYSCALL_CLOSE = 3,
    SYSCALL_EXEC = 4,
    SYSCALL_FORK = 5,
    SYSCALL_WAIT = 6,
    SYSCALL_EXIT = 7,
    SYSCALL_GETPID = 8,
    SYSCALL_GETPPID = 9,
    SYSCALL_KILL = 10,
    SYSCALL_SIGNAL = 11,
    SYSCALL_IOCTL = 12,
    SYSCALL_LSEEK = 13,
    SYSCALL_STAT = 14,
    SYSCALL_FSTAT = 15,
    SYSCALL_CHDIR = 16,
    SYSCALL_CHMOD = 17,
    SYSCALL_CHOWN = 18,
    SYSCALL_MKDIR = 19,
    SYSCALL_RMDIR = 20,
    SYSCALL_UNLINK = 21,
    SYSCALL_LINK = 22,
    SYSCALL_SYMLINK = 23,
    SYSCALL_READLINK = 24,
    SYSCALL_GETCWD = 25,
    SYSCALL_GETENV = 26,
    SYSCALL_SETENV = 27,
    SYSCALL_UNSETENV = 28,
    SYSCALL_MMAP = 29,
    SYSCALL_MUNMAP = 30,
    SYSCALL_MPROTECT = 31,
    SYSCALL_BRK = 32,
    SYSCALL_SBRK = 33,
    SYSCALL_MLOCK = 34,
    SYSCALL_MUNLOCK = 35,
    SYSCALL_CLONE = 36,
    SYSCALL_THREAD_CREATE = 37,
    SYSCALL_THREAD_EXIT = 38,
    SYSCALL_THREAD_JOIN = 39,
    SYSCALL_THREAD_KILL = 40,
    SYSCALL_MUTEX_CREATE = 41,
    SYSCALL_MUTEX_LOCK = 42,
    SYSCALL_MUTEX_UNLOCK = 43,
    SYSCALL_SEMAPHORE_CREATE = 44,
    SYSCALL_SEMAPHORE_WAIT = 45,
    SYSCALL_SEMAPHORE_SIGNAL = 46,
    SYSCALL_PIPE = 47,
    SYSCALL_SOCKET = 48,
    SYSCALL_BIND = 49,
    SYSCALL_LISTEN = 50,
    SYSCALL_ACCEPT = 51,
    SYSCALL_CONNECT = 52,
    SYSCALL_SEND = 53,
    SYSCALL_RECV = 54,
    SYSCALL_SENDTO = 55,
    SYSCALL_RECVFROM = 56,
    SYSCALL_SHUTDOWN = 57,
    SYSCALL_SETSOCKOPT = 58,
    SYSCALL_GETSOCKOPT = 59,
    SYSCALL_GETHOSTNAME = 60,
    SYSCALL_SETHOSTNAME = 61,
    SYSCALL_GETTIME = 62,
    SYSCALL_SETTIME = 63,
    SYSCALL_SLEEP = 64,
    SYSCALL_USLEEP = 65,
    SYSCALL_TIMES = 66,
    SYSCALL_SYSINFO = 67,
    SYSCALL_PRCTL = 68,
    SYSCALL_UNAME = 69,
    SYSCALL_SYSCTL = 70,
    SYSCALL_REBOOT = 71,
    SYSCALL_SHUTDOWN_SYS = 72,
    SYSCALL_MOUNT = 73,
    SYSCALL_UMOUNT = 74,
    SYSCALL_DUP = 75,
    SYSCALL_DUP2 = 76,
    SYSCALL_FCNTL = 77,
    SYSCALL_POLL = 78,
    SYSCALL_SELECT = 79,
    SYSCALL_EPOLL_CREATE = 80,
    SYSCALL_EPOLL_CTL = 81,
    SYSCALL_EPOLL_WAIT = 82,
    SYSCALL_MAX = 83
} SyscallNumber;

typedef struct
{
    uint64_t args[MAX_SYSCALL_ARGS];
    uint32_t argc;
    int32_t return_value;
    int32_t error_code;
} SyscallContext;

typedef int64_t (*syscall_handler_t)(SyscallContext *ctx);

typedef struct
{
    SyscallNumber syscall_num;
    syscall_handler_t handler;
    const char *name;
    uint32_t call_count;
    uint64_t total_time_us;
} SyscallDescriptor;

typedef struct
{
    SyscallDescriptor descriptors[MAX_SYSCALLS];
    uint32_t handler_count;
    uint64_t total_syscalls;
} SyscallHandler;

SyscallHandler *syscall_init(void);
void syscall_cleanup(SyscallHandler *handler);
int syscall_register(SyscallHandler *handler, SyscallNumber num, syscall_handler_t func, const char *name);
int64_t syscall_invoke(SyscallHandler *handler, SyscallNumber num, SyscallContext *ctx);
uint32_t syscall_get_handler_count(SyscallHandler *handler);
uint64_t syscall_get_total_calls(SyscallHandler *handler);

int64_t syscall_read(SyscallContext *ctx);
int64_t syscall_write(SyscallContext *ctx);
int64_t syscall_open(SyscallContext *ctx);
int64_t syscall_close(SyscallContext *ctx);
int64_t syscall_fork(SyscallContext *ctx);
int64_t syscall_exit(SyscallContext *ctx);
int64_t syscall_exec(SyscallContext *ctx);
int64_t syscall_wait(SyscallContext *ctx);
int64_t syscall_getpid(SyscallContext *ctx);
int64_t syscall_kill(SyscallContext *ctx);

#endif
