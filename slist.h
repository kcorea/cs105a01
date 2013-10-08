#ifndef __SLIST_H__
#define __SLIST_H__

#include <sys/stat.h>

typedef struct st_slist *slist_ref;
typedef struct st_node *slist_node;

slist_ref new_slist (void);
slist_node new_slistnode (struct stat *, char *);
void insert_slist (slist_ref, slist_node);

bool has_empty_dirstack (slist_ref);
char *pop_slistdir (slist_ref);
void push_slistdir (slist_ref, slist_node);
void stack_slistdir (slist_ref);

void free_slist (slist_ref);
void free_slistnode (slist_node);
void print_slist_long (slist_ref);
void print_slist_short (slist_ref);
bool is_dir (slist_node);
bool is_hidden (slist_node);

#endif
