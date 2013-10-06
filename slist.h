#ifndef __SLIST_H__
#define __SLIST_H__

#include <sys/stat.h>

typedef struct st_slist *slist_ref;
typedef struct st_node *slist_node;

slist_ref new_slist (void);
slist_node new_slistnode (struct stat *, char *);
void insert_slist (slist_ref, slist_node);
void free_slist (slist_ref);
void print_slist_long (slist_ref);
void print_slist_short (slist_ref);

#endif
