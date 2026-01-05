#ifndef MERCURIAL_H
#define MERCURIAL_H

#include <stdint.h>

typedef struct
{
    const char *repository_url;
    const char *destination;
    const char *branch;
} mercurial_clone_options_t;

typedef struct
{
    const char *commit_message;
    const char *author;
    uint32_t timestamp;
} mercurial_commit_info_t;

int mercurial_init(void);
int mercurial_clone(const char *repository_url, const char *destination);
int mercurial_clone_with_options(mercurial_clone_options_t *options);
int mercurial_commit(mercurial_commit_info_t *commit_info);
int mercurial_push(const char *remote);
int mercurial_pull(const char *remote);
int mercurial_get_log(char *log_buffer, uint32_t buffer_size);

#endif
