#include <string.h>
#include "compage_llist.h"

void llist_entry_add(compage_t **head, compage_t *entry){
  entry->next = *head;
  *head = entry;
}

compage_t* llist_entry_remove(compage_t **indirect){
  compage_t *entry;

  entry = *indirect;
  *indirect = entry->next;
  return entry;
}

compage_t* llist_entry_find_by_name(compage_t *head, const char *name){
  compage_t *it = head;

  while(it != NULL){
    if(strcmp(it->name, name) == 0){
      return it;
    }

    it = it->next;
  }

  return NULL;
}

compage_t* llist_entry_find_by_sid(compage_t *head, const char *sid){
  compage_t *it = head;

  while(it != NULL){
    if(strcmp(it->sid, sid) == 0){
      return it;
    }

    it = it->next;
  }

  return NULL;
}

compage_t* llist_entry_find_by_id(compage_t *head, unsigned id){
  compage_t *it = head;

  while(it != NULL){
    if(it->id == id){
      return it;
    }

    it = it->next;
  }

  return NULL;
}
