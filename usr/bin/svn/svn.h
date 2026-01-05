#ifndef SVN_H
#define SVN_H

#include <stdint.h>

typedef struct
{
    const char *repository_url;
    const char *working_copy;
    const char *username;
    const char *password;
} svn_checkout_options_t;

typedef struct
{
    const char *log_message;
    const char *author;
    uint32_t revision_number;
    uint64_t timestamp;
} svn_commit_info_t;

int svn_init(void);
int svn_checkout(const char *repository_url, const char *working_copy);
int svn_checkout_with_options(svn_checkout_options_t *options);
int svn_commit(svn_commit_info_t *commit_info);
int svn_update(const char *working_copy);
int svn_get_log(const char *path, char *log_buffer, uint32_t buffer_size);

#endif
