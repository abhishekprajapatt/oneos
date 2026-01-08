#ifndef FILESYSTEM_CORE_H
#define FILESYSTEM_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_FILENAME 256
#define MAX_PATH 512
#define MAX_FILES 10000

typedef enum
{
    FS_TYPE_FILE,
    FS_TYPE_DIR,
    FS_TYPE_SYMLINK,
    FS_TYPE_DEVICE
} FileType;

typedef enum
{
    PERM_OWNER_READ = 0400,
    PERM_OWNER_WRITE = 0200,
    PERM_OWNER_EXEC = 0100,
    PERM_GROUP_READ = 040,
    PERM_GROUP_WRITE = 020,
    PERM_GROUP_EXEC = 010,
    PERM_OTHER_READ = 04,
    PERM_OTHER_WRITE = 02,
    PERM_OTHER_EXEC = 01
} FilePermission;

typedef struct
{
    uint32_t inode;
    char name[MAX_FILENAME];
    FileType type;
    uint32_t permissions;
    uint32_t owner_uid;
    uint32_t owner_gid;
    uint64_t size;
    time_t created_time;
    time_t modified_time;
    time_t accessed_time;
    uint32_t link_count;
} FileEntry;

typedef struct
{
    FileEntry *entries;
    uint32_t entry_count;
    uint32_t max_entries;
    char mount_point[MAX_PATH];
} FileSystem;

FileSystem *filesystem_init(const char *mount_point);
void filesystem_cleanup(FileSystem *fs);
uint32_t filesystem_create_file(FileSystem *fs, const char *name, FileType type, uint32_t permissions);
int filesystem_delete_file(FileSystem *fs, uint32_t inode);
FileEntry *filesystem_find_file(FileSystem *fs, const char *name);
int filesystem_write_file(FileSystem *fs, uint32_t inode, const void *data, uint64_t size);
int filesystem_read_file(FileSystem *fs, uint32_t inode, void *buffer, uint64_t size);
int filesystem_list_files(FileSystem *fs, FileEntry **entries);

#endif
