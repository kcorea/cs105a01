#ifndef __DQUEUE_H__
#define __DQUEUE_H__

#include <stdbool.h>

typedef struct st_queue *queue_ref;

queue_ref new_queue (void);
void free_queue (queue_ref);
void insert_queue (queue_ref, char *);
char *remove_queue (queue_ref);
bool is_empty_queue (queue_ref);

#endif
