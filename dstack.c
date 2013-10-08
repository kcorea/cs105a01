#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debugf.h"
#include "dstack.h"

static char *stack_tag = "struct st_stack";

typedef struct stacknode *stack_node;

struct stacknode {
   char *dir;
   stack_node link;
};

struct st_stack {
   char *tag;
   stack_node top;
};

bool is_stack (stack_ref stack) {
   return stack != NULL && stack->tag == stack_tag;
}

stack_ref new_stack (void) {
   stack_ref new = malloc (sizeof (struct st_stack));
   assert (new != NULL);
   new->tag = stack_tag;
   new->top = NULL;
   return new;
}

void free_stack (stack_ref stack) {
   assert (is_stack (stack));
   stack_node curr = stack->top;
   for (;;) {
      if (curr == NULL) break;
      stack_node old = curr;
      curr = curr->link;
      free (old->dir);
      free (old);
   }
   memset (stack, 0, sizeof (struct st_stack));
   free (stack);
}

void push_stack (stack_ref stack, char *dir) {
   assert (is_stack (stack));
   stack_node new = malloc (sizeof (struct stacknode));
   new->dir = strdup (dir);
   new->link = stack->top;
   stack->top = new;
}

char *pop_stack (stack_ref stack) {
   assert (is_stack (stack));
   assert (! is_empty_stack (stack));
   stack_node tmp = stack->top;
   stack->top = stack->top->link;
   return tmp->dir;
}

bool is_empty_stack (stack_ref stack) {
   assert (is_stack (stack));
   return stack->top == NULL;
}

