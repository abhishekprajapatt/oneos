#include "fs_journal.h"
#include <stdlib.h>
#include <string.h>

FilesystemJournal *journal_init(JournalMode mode)
{
    FilesystemJournal *journal = (FilesystemJournal *)malloc(sizeof(FilesystemJournal));
    if (!journal)
        return NULL;

    memset(journal, 0, sizeof(FilesystemJournal));
    journal->mode = mode;
    journal->entry_count = 0;
    journal->transaction_count = 0;
    journal->current_transaction_id = 1;
    journal->total_transactions = 0;
    journal->recovered_transactions = 0;
    journal->recovery_needed = false;

    return journal;
}

void journal_cleanup(FilesystemJournal *journal)
{
    if (!journal)
        return;

    for (uint32_t i = 0; i < journal->transaction_count; i++)
    {
        if (journal->transactions[i].state == TRANS_STATE_RUNNING ||
            journal->transactions[i].state == TRANS_STATE_COMMITTING)
        {
            journal_abort_transaction(journal, journal->transactions[i].transaction_id);
        }
    }

    memset(journal, 0, sizeof(FilesystemJournal));
    free(journal);
}

uint32_t journal_begin_transaction(FilesystemJournal *journal)
{
    if (!journal || journal->transaction_count >= MAX_JOURNAL_TRANSACTIONS)
        return 0;

    Transaction *trans = &journal->transactions[journal->transaction_count];
    trans->transaction_id = journal->current_transaction_id++;
    trans->state = TRANS_STATE_RUNNING;
    trans->start_time = time(NULL);
    trans->commit_time = 0;
    trans->entry_count = 0;
    trans->in_progress = true;

    journal->transaction_count++;
    journal->total_transactions++;

    return trans->transaction_id;
}

int journal_add_entry(FilesystemJournal *journal, uint32_t trans_id, JournalEntryType type,
                      uint64_t block_num, const uint8_t *data, uint32_t data_len)
{
    if (!journal || journal->entry_count >= MAX_JOURNAL_ENTRIES)
        return -1;
    if (data_len > JOURNAL_ENTRY_SIZE)
        return -1;

    Transaction *trans = journal_get_transaction(journal, trans_id);
    if (!trans || trans->state != TRANS_STATE_RUNNING)
        return -1;

    JournalEntry *entry = &journal->entries[journal->entry_count];
    entry->entry_id = journal->entry_count + 1;
    entry->type = type;
    entry->transaction_id = trans_id;
    entry->block_number = block_num;
    entry->data_length = data_len;
    entry->timestamp = time(NULL);
    entry->is_synced = false;

    if (data && data_len > 0)
    {
        memcpy(entry->data, data, data_len);
        entry->checksum = journal_calculate_checksum(data, data_len);
    }
    else
    {
        entry->checksum = 0;
    }

    trans->entry_ids[trans->entry_count] = entry->entry_id;
    trans->entry_count++;

    journal->entry_count++;
    return entry->entry_id;
}

int journal_commit_transaction(FilesystemJournal *journal, uint32_t trans_id)
{
    if (!journal)
        return -1;

    Transaction *trans = journal_get_transaction(journal, trans_id);
    if (!trans || trans->state != TRANS_STATE_RUNNING)
        return -1;

    trans->state = TRANS_STATE_COMMITTING;
    trans->commit_time = time(NULL);

    for (uint32_t i = 0; i < trans->entry_count; i++)
    {
        JournalEntry *entry = journal_get_entry(journal, trans->entry_ids[i]);
        if (entry)
        {
            entry->is_synced = true;
        }
    }

    trans->state = TRANS_STATE_COMMITTED;
    trans->in_progress = false;

    return 0;
}

int journal_abort_transaction(FilesystemJournal *journal, uint32_t trans_id)
{
    if (!journal)
        return -1;

    Transaction *trans = journal_get_transaction(journal, trans_id);
    if (!trans)
        return -1;

    trans->state = TRANS_STATE_ABORTED;
    trans->in_progress = false;

    for (uint32_t i = 0; i < trans->entry_count; i++)
    {
        JournalEntry *entry = journal_get_entry(journal, trans->entry_ids[i]);
        if (entry)
        {
            entry->is_synced = false;
        }
    }

    return 0;
}

int journal_checkpoint(FilesystemJournal *journal)
{
    if (!journal)
        return -1;

    for (uint32_t i = 0; i < journal->transaction_count; i++)
    {
        if (journal->transactions[i].state == TRANS_STATE_COMMITTED &&
            !journal->transactions[i].in_progress)
        {
            memset(&journal->transactions[i], 0, sizeof(Transaction));
        }
    }

    uint32_t new_trans_count = 0;
    for (uint32_t i = 0; i < journal->transaction_count; i++)
    {
        if (journal->transactions[i].transaction_id > 0)
        {
            memmove(&journal->transactions[new_trans_count],
                    &journal->transactions[i],
                    sizeof(Transaction));
            new_trans_count++;
        }
    }
    journal->transaction_count = new_trans_count;

    return 0;
}

int journal_recover(FilesystemJournal *journal)
{
    if (!journal)
        return -1;

    journal->recovery_needed = false;
    uint32_t recovered_count = 0;

    for (uint32_t i = 0; i < journal->transaction_count; i++)
    {
        Transaction *trans = &journal->transactions[i];
        if (trans->state == TRANS_STATE_RUNNING || trans->state == TRANS_STATE_COMMITTING)
        {
            for (uint32_t j = 0; j < trans->entry_count; j++)
            {
                JournalEntry *entry = journal_get_entry(journal, trans->entry_ids[j]);
                if (entry && journal_verify_checksum(entry) == 0)
                {
                    if (trans->state == TRANS_STATE_COMMITTING)
                    {
                        entry->is_synced = true;
                        recovered_count++;
                    }
                }
            }

            if (trans->state == TRANS_STATE_COMMITTING)
            {
                trans->state = TRANS_STATE_COMMITTED;
            }
            else
            {
                trans->state = TRANS_STATE_ABORTED;
            }
        }
    }

    journal->recovered_transactions = recovered_count;
    return 0;
}

int journal_sync_to_disk(FilesystemJournal *journal)
{
    if (!journal)
        return -1;

    uint32_t synced_count = 0;
    for (uint32_t i = 0; i < journal->entry_count; i++)
    {
        if (!journal->entries[i].is_synced)
        {
            journal->entries[i].is_synced = true;
            synced_count++;
        }
    }

    return synced_count;
}

JournalEntry *journal_get_entry(FilesystemJournal *journal, uint32_t entry_id)
{
    if (!journal)
        return NULL;

    for (uint32_t i = 0; i < journal->entry_count; i++)
    {
        if (journal->entries[i].entry_id == entry_id)
        {
            return &journal->entries[i];
        }
    }
    return NULL;
}

Transaction *journal_get_transaction(FilesystemJournal *journal, uint32_t trans_id)
{
    if (!journal)
        return NULL;

    for (uint32_t i = 0; i < journal->transaction_count; i++)
    {
        if (journal->transactions[i].transaction_id == trans_id)
        {
            return &journal->transactions[i];
        }
    }
    return NULL;
}

uint32_t journal_get_pending_transactions(FilesystemJournal *journal)
{
    if (!journal)
        return 0;

    uint32_t pending = 0;
    for (uint32_t i = 0; i < journal->transaction_count; i++)
    {
        if (journal->transactions[i].in_progress)
        {
            pending++;
        }
    }
    return pending;
}

int journal_set_mode(FilesystemJournal *journal, JournalMode mode)
{
    if (!journal)
        return -1;
    if (journal_get_pending_transactions(journal) > 0)
        return -1;

    journal->mode = mode;
    return 0;
}

JournalMode journal_get_mode(FilesystemJournal *journal)
{
    if (!journal)
        return JOURNAL_MODE_WRITEBACK;
    return journal->mode;
}

uint32_t journal_calculate_checksum(const uint8_t *data, uint32_t len)
{
    if (!data || len == 0)
        return 0;

    uint32_t checksum = 0;
    for (uint32_t i = 0; i < len; i++)
    {
        checksum = ((checksum << 1) | (checksum >> 31)) ^ data[i];
    }
    return checksum;
}

int journal_verify_checksum(JournalEntry *entry)
{
    if (!entry)
        return -1;

    uint32_t calculated = journal_calculate_checksum(entry->data, entry->data_length);
    return (calculated == entry->checksum) ? 0 : -1;
}
