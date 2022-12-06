#ifndef _COMPAGE_LLIST_H_
#define _COMPAGE_LLIST_H_

#include "compage_types.h"

void llist_entry_add(compage_t **head, compage_t *entry);
compage_t* llist_entry_remove(compage_t **indirect);

compage_t* llist_entry_find_by_name(compage_t *head, const char *name);
compage_t* llist_entry_find_by_sid(compage_t *head, const char *sid);
compage_t* llist_entry_find_by_id(compage_t *head, unsigned id);

#endif
