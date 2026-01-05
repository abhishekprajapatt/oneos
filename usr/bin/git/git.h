#ifndef GIT_H
#define GIT_H

#include <stdint.h>

typedef struct
{
    const char *author_name;
    const char *author_email;
    const char *commit_message;
    uint32_t timestamp;
} git_commit_info_t;

typedef struct
{
    const char *remote_url;
    const char *branch;
    const char *commit_hash;
} git_branch_info_t;

int git_init(void);
int git_clone(const char *repository_url, const char *destination);
int git_commit(git_commit_info_t *commit_info);
int git_push(const char *remote, const char *branch);
int git_pull(const char *remote, const char *branch);
int git_get_current_branch(char *branch_name, uint32_t max_len);
int git_get_commit_history(char *history, uint32_t max_len);

#endif
