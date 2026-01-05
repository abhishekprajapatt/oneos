#ifndef USR_BIN_H
#define USR_BIN_H

#include <stdint.h>

typedef struct
{
    uint32_t bin_id;
    const char *binary_name;
    const char *binary_path;
    uint32_t permissions;
    uint64_t size;
} usr_bin_entry_t;

typedef struct
{
    uint32_t total_binaries;
    uint32_t executable_binaries;
    uint64_t total_size;
} usr_bin_state_t;

int usr_bin_init(void);
int usr_bin_list_binaries(usr_bin_entry_t *binaries, uint32_t *count);
int usr_bin_get_binary(const char *name, usr_bin_entry_t *binary);
int usr_bin_execute(const char *binary_name, const char **args, uint32_t arg_count);
int usr_bin_verify_checksum(const char *binary_name, uint32_t *checksum);
int usr_bin_get_state(usr_bin_state_t *state);

#endif
