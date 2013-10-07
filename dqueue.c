#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debugf.h"
#include "dqueue.h"

static char *queue_tag = "struct st_queue";

typedef struct queuenode *queue_node;

struct queuenode {
   char *dir;
   queue_node link;
};

struct st_queue {
   char *tag;
   queue_node top;
   queue_node front;
   queue_node rear;
};

bool is_queue (queue_ref queue) {
   return queue != NULL && queue->tag == queue_tag;
}

queue_ref new_queue (void) {
   queue_ref new = malloc (sizeof (struct st_queue));
   assert (new != NULL);
   new->tag = queue_tag;
   new->top = NULL;
   new->front = NULL;
   new->rear = NULL;
   return new;
}

void free_queue (queue_ref queue) {
   assert (is_queue (queue));
   assert (is_empty_queue (queue));
   queue_node curr = queue->top;
   for (;;) {
      if (curr == NULL) break;
      queue_node old = curr;
      curr = curr->link;
      free (old->dir);
      free (old);
   }
   memset (queue, 0, sizeof (struct st_queue));
   free (queue);
}

void insert_queue (queue_ref queue, char *dir) {
   assert (is_queue (queue));
   queue_node new = malloc (sizeof (struct queuenode));
   new->dir = strdup (dir);
   new->link = NULL;
   if (queue->rear == NULL) {
      queue->top = new;
      queue->front = new;
   }else {
      queue->rear->link = new;
   }
   queue->rear = new;
}

char *remove_queue (queue_ref queue) {
   assert (is_queue (queue));
   assert (! is_empty_queue (queue));
   queue_node tmp = queue->front;
   queue->front = queue->front->link;
   if (queue->front == NULL) queue->rear = NULL;
   char *dir = tmp->dir;
   return dir;
}

bool is_empty_queue (queue_ref queue) {
   assert (is_queue (queue));
   return queue->front == NULL;
}

