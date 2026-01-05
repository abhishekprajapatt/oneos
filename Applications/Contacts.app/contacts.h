#ifndef CONTACTS_H
#define CONTACTS_H

#include <stdint.h>

typedef struct
{
    uint32_t contact_id;
    const char *first_name;
    const char *last_name;
    const char *email;
    const char *phone;
    const char *address;
} contact_info_t;

typedef struct
{
    uint32_t total_contacts;
    uint32_t favorites;
    uint32_t groups;
    uint32_t last_updated;
} contacts_stats_t;

int contacts_init(void);
int contacts_create(contact_info_t *contact);
int contacts_delete(uint32_t contact_id);
int contacts_update(contact_info_t *contact);
int contacts_get(uint32_t contact_id, contact_info_t *contact);
int contacts_list(contact_info_t *contacts, uint32_t *count);
int contacts_search(const char *query, contact_info_t *results, uint32_t *count);
int contacts_get_stats(contacts_stats_t *stats);

#endif
