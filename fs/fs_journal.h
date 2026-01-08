#ifndef FS_JOURNAL_H
#define FS_JOURNAL_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_JOURNAL_ENTRIES 4096
#define MAX_JOURNAL_TRANSACTIONS 512
#define JOURNAL_ENTRY_SIZE 512

typedef enum
{
    JOURNAL_MODE_WRITEBACK = 0,
    JOURNAL_MODE_ORDERED = 1,
    JOURNAL_MODE_DATA = 2
} JournalMode;

typedef enum
{
    JOURNAL_ENTRY_START = 0,
    JOURNAL_ENTRY_METADATA = 1,
    JOURNAL_ENTRY_DATA = 2,
    JOURNAL_ENTRY_COMMIT = 3,
    JOURNAL_ENTRY_REVOKE = 4
} JournalEntryType;

typedef enum
{
    TRANS_STATE_IDLE = 0,
    TRANS_STATE_RUNNING = 1,
    TRANS_STATE_COMMITTING = 2,
    TRANS_STATE_COMMITTED = 3,
    TRANS_STATE_ABORTED = 4
} TransactionState;

typedef struct
{
    uint32_t entry_id;
    JournalEntryType type;
    uint32_t transaction_id;
    uint64_t block_number;
    uint32_t data_length;
    uint8_t data[JOURNAL_ENTRY_SIZE];
    uint32_t checksum;
    time_t timestamp;
    bool is_synced;
} JournalEntry;

typedef struct
{
    uint32_t transaction_id;
    TransactionState state;
    time_t start_time;
    time_t commit_time;
    uint32_t entry_count;
    uint32_t entry_ids[256];
    bool in_progress;
} Transaction;

typedef struct
{
    JournalEntry entries[MAX_JOURNAL_ENTRIES];
    uint32_t entry_count;
    Transaction transactions[MAX_JOURNAL_TRANSACTIONS];
    uint32_t transaction_count;
    uint32_t current_transaction_id;
    JournalMode mode;
    uint64_t total_transactions;
    uint64_t recovered_transactions;
    bool recovery_needed;
} FilesystemJournal;

FilesystemJournal *journal_init(JournalMode mode);
void journal_cleanup(FilesystemJournal *journal);

uint32_t journal_begin_transaction(FilesystemJournal *journal);
int journal_add_entry(FilesystemJournal *journal, uint32_t trans_id, JournalEntryType type,
                      uint64_t block_num, const uint8_t *data, uint32_t data_len);
int journal_commit_transaction(FilesystemJournal *journal, uint32_t trans_id);
int journal_abort_transaction(FilesystemJournal *journal, uint32_t trans_id);

int journal_checkpoint(FilesystemJournal *journal);
int journal_recover(FilesystemJournal *journal);
int journal_sync_to_disk(FilesystemJournal *journal);

JournalEntry *journal_get_entry(FilesystemJournal *journal, uint32_t entry_id);
Transaction *journal_get_transaction(FilesystemJournal *journal, uint32_t trans_id);
uint32_t journal_get_pending_transactions(FilesystemJournal *journal);

int journal_set_mode(FilesystemJournal *journal, JournalMode mode);
JournalMode journal_get_mode(FilesystemJournal *journal);

uint32_t journal_calculate_checksum(const uint8_t *data, uint32_t len);
int journal_verify_checksum(JournalEntry *entry);

#endif
