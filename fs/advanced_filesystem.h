#ifndef ADVANCED_FILESYSTEM_H
#define ADVANCED_FILESYSTEM_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_SNAPSHOTS 100
#define MAX_FILE_VERSIONS 50
#define MAX_CACHED_BLOCKS 10000

typedef enum
{
    COMPRESSION_NONE,
    COMPRESSION_ZLIB,
    COMPRESSION_LZFSE,
    COMPRESSION_LZ4
} CompressionType;

typedef struct
{
    uint32_t snapshot_id;
    time_t created_time;
    char name[256];
    uint64_t size;
    bool is_readonly;
} Snapshot;

typedef struct
{
    uint32_t version;
    time_t timestamp;
    uint64_t size;
    char checksum[64];
} FileVersion;

typedef struct
{
    uint32_t block_id;
    void *data;
    uint64_t size;
    CompressionType compression;
    uint32_t reference_count;
    time_t last_accessed;
} CacheBlock;

typedef struct
{
    uint32_t inode;
    char name[256];
    uint64_t size;
    CompressionType compression_type;
    bool encryption_enabled;
    FileVersion versions[MAX_FILE_VERSIONS];
    uint32_t version_count;
    uint64_t allocated_blocks;
} AdvancedFile;

typedef struct
{
    AdvancedFile *files;
    uint32_t file_count;
    uint32_t max_files;
    Snapshot snapshots[MAX_SNAPSHOTS];
    uint32_t snapshot_count;
    CacheBlock cache[MAX_CACHED_BLOCKS];
    uint32_t cache_count;
    float compression_ratio;
} AdvancedFilesystem;

AdvancedFilesystem *afs_init(void);
void afs_cleanup(AdvancedFilesystem *afs);
uint32_t afs_create_snapshot(AdvancedFilesystem *afs, const char *name);
int afs_restore_snapshot(AdvancedFilesystem *afs, uint32_t snapshot_id);
int afs_enable_compression(AdvancedFilesystem *afs, uint32_t inode, CompressionType type);
int afs_enable_encryption(AdvancedFilesystem *afs, uint32_t inode, const char *key);
int afs_file_version_save(AdvancedFilesystem *afs, uint32_t inode);
int afs_file_version_restore(AdvancedFilesystem *afs, uint32_t inode, uint32_t version);
int afs_cache_block(AdvancedFilesystem *afs, uint32_t block_id, void *data, uint64_t size);
float afs_get_compression_ratio(AdvancedFilesystem *afs);

#endif
