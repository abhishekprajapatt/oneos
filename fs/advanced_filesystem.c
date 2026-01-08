#include "advanced_filesystem.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

AdvancedFilesystem *afs_init(void)
{
    AdvancedFilesystem *afs = (AdvancedFilesystem *)malloc(sizeof(AdvancedFilesystem));
    if (!afs)
        return NULL;

    afs->max_files = 10000;
    afs->file_count = 0;
    afs->snapshot_count = 0;
    afs->cache_count = 0;
    afs->compression_ratio = 1.0f;

    afs->files = (AdvancedFile *)malloc(afs->max_files * sizeof(AdvancedFile));
    if (!afs->files)
    {
        free(afs);
        return NULL;
    }

    memset(afs->files, 0, afs->max_files * sizeof(AdvancedFile));
    memset(afs->snapshots, 0, sizeof(afs->snapshots));
    memset(afs->cache, 0, sizeof(afs->cache));

    return afs;
}

void afs_cleanup(AdvancedFilesystem *afs)
{
    if (!afs)
        return;

    for (uint32_t i = 0; i < afs->cache_count; i++)
    {
        if (afs->cache[i].data)
        {
            free(afs->cache[i].data);
        }
    }

    if (afs->files)
    {
        free(afs->files);
    }
    free(afs);
}

uint32_t afs_create_snapshot(AdvancedFilesystem *afs, const char *name)
{
    if (!afs || afs->snapshot_count >= MAX_SNAPSHOTS || !name)
        return 0;

    Snapshot *snap = &afs->snapshots[afs->snapshot_count];
    snap->snapshot_id = afs->snapshot_count + 1;
    strncpy(snap->name, name, 255);
    snap->created_time = time(NULL);
    snap->is_readonly = false;
    snap->size = 0;

    for (uint32_t i = 0; i < afs->file_count; i++)
    {
        snap->size += afs->files[i].size;
    }

    afs->snapshot_count++;
    return snap->snapshot_id;
}

int afs_restore_snapshot(AdvancedFilesystem *afs, uint32_t snapshot_id)
{
    if (!afs || snapshot_id == 0 || snapshot_id > afs->snapshot_count)
        return -1;

    Snapshot *snap = &afs->snapshots[snapshot_id - 1];
    snap->is_readonly = true;

    return 0;
}

int afs_enable_compression(AdvancedFilesystem *afs, uint32_t inode, CompressionType type)
{
    if (!afs)
        return -1;

    for (uint32_t i = 0; i < afs->file_count; i++)
    {
        if (afs->files[i].inode == inode)
        {
            afs->files[i].compression_type = type;
            afs->compression_ratio *= 0.7f;
            return 0;
        }
    }

    return -1;
}

int afs_enable_encryption(AdvancedFilesystem *afs, uint32_t inode, const char *key)
{
    if (!afs || !key)
        return -1;

    for (uint32_t i = 0; i < afs->file_count; i++)
    {
        if (afs->files[i].inode == inode)
        {
            afs->files[i].encryption_enabled = true;
            return 0;
        }
    }

    return -1;
}

int afs_file_version_save(AdvancedFilesystem *afs, uint32_t inode)
{
    if (!afs)
        return -1;

    for (uint32_t i = 0; i < afs->file_count; i++)
    {
        if (afs->files[i].inode == inode && afs->files[i].version_count < MAX_FILE_VERSIONS)
        {
            FileVersion *ver = &afs->files[i].versions[afs->files[i].version_count];
            ver->version = afs->files[i].version_count + 1;
            ver->timestamp = time(NULL);
            ver->size = afs->files[i].size;
            strcpy(ver->checksum, "pending");

            afs->files[i].version_count++;
            return ver->version;
        }
    }

    return -1;
}

int afs_file_version_restore(AdvancedFilesystem *afs, uint32_t inode, uint32_t version)
{
    if (!afs)
        return -1;

    for (uint32_t i = 0; i < afs->file_count; i++)
    {
        if (afs->files[i].inode == inode && version <= afs->files[i].version_count)
        {
            return 0;
        }
    }

    return -1;
}

int afs_cache_block(AdvancedFilesystem *afs, uint32_t block_id, void *data, uint64_t size)
{
    if (!afs || afs->cache_count >= MAX_CACHED_BLOCKS || !data || size == 0)
        return -1;

    CacheBlock *block = &afs->cache[afs->cache_count];
    block->block_id = block_id;
    block->data = malloc(size);
    if (!block->data)
        return -1;

    memcpy(block->data, data, size);
    block->size = size;
    block->compression = COMPRESSION_NONE;
    block->reference_count = 1;
    block->last_accessed = time(NULL);

    afs->cache_count++;
    return 0;
}

float afs_get_compression_ratio(AdvancedFilesystem *afs)
{
    if (!afs)
        return 1.0f;
    return afs->compression_ratio;
}
