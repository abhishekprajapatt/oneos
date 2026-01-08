#include "filesystem_core.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

FileSystem *filesystem_init(const char *mount_point)
{
    FileSystem *fs = (FileSystem *)malloc(sizeof(FileSystem));
    if (!fs)
        return NULL;

    fs->max_entries = MAX_FILES;
    fs->entry_count = 0;
    fs->entries = (FileEntry *)malloc(MAX_FILES * sizeof(FileEntry));

    if (!fs->entries)
    {
        free(fs);
        return NULL;
    }

    if (mount_point)
    {
        strncpy(fs->mount_point, mount_point, MAX_PATH - 1);
    }
    else
    {
        strcpy(fs->mount_point, "/");
    }

    memset(fs->entries, 0, MAX_FILES * sizeof(FileEntry));

    return fs;
}

void filesystem_cleanup(FileSystem *fs)
{
    if (!fs)
        return;
    if (fs->entries)
    {
        free(fs->entries);
    }
    free(fs);
}

uint32_t filesystem_create_file(FileSystem *fs, const char *name, FileType type, uint32_t permissions)
{
    if (!fs || fs->entry_count >= MAX_FILES || !name)
        return 0;

    FileEntry *entry = &fs->entries[fs->entry_count];
    entry->inode = fs->entry_count + 1;
    strncpy(entry->name, name, MAX_FILENAME - 1);
    entry->type = type;
    entry->permissions = permissions;
    entry->owner_uid = 0;
    entry->owner_gid = 0;
    entry->size = 0;
    entry->created_time = time(NULL);
    entry->modified_time = time(NULL);
    entry->accessed_time = time(NULL);
    entry->link_count = 1;

    fs->entry_count++;
    return entry->inode;
}

int filesystem_delete_file(FileSystem *fs, uint32_t inode)
{
    if (!fs)
        return -1;

    for (uint32_t i = 0; i < fs->entry_count; i++)
    {
        if (fs->entries[i].inode == inode)
        {
            for (uint32_t j = i; j < fs->entry_count - 1; j++)
            {
                fs->entries[j] = fs->entries[j + 1];
            }
            fs->entry_count--;
            return 0;
        }
    }

    return -1;
}

FileEntry *filesystem_find_file(FileSystem *fs, const char *name)
{
    if (!fs || !name)
        return NULL;

    for (uint32_t i = 0; i < fs->entry_count; i++)
    {
        if (strcmp(fs->entries[i].name, name) == 0)
        {
            return &fs->entries[i];
        }
    }

    return NULL;
}

int filesystem_write_file(FileSystem *fs, uint32_t inode, const void *data, uint64_t size)
{
    if (!fs || !data || size == 0)
        return -1;

    for (uint32_t i = 0; i < fs->entry_count; i++)
    {
        if (fs->entries[i].inode == inode && fs->entries[i].type == FS_TYPE_FILE)
        {
            fs->entries[i].size += size;
            fs->entries[i].modified_time = time(NULL);
            return 0;
        }
    }

    return -1;
}

int filesystem_read_file(FileSystem *fs, uint32_t inode, void *buffer, uint64_t size)
{
    if (!fs || !buffer || size == 0)
        return -1;

    for (uint32_t i = 0; i < fs->entry_count; i++)
    {
        if (fs->entries[i].inode == inode && fs->entries[i].type == FS_TYPE_FILE)
        {
            fs->entries[i].accessed_time = time(NULL);
            return fs->entries[i].size;
        }
    }

    return -1;
}

int filesystem_list_files(FileSystem *fs, FileEntry **entries)
{
    if (!fs || !entries)
        return 0;
    *entries = fs->entries;
    return fs->entry_count;
}
