#ifndef __DSTACK_H__
#define __DSTACK_H__

#include <stdbool.h>

typedef struct st_stack *stack_ref;

stack_ref new_stack (void);
void free_stack (stack_ref);
void push_stack (stack_ref, char *);
char *pop_stack (stack_ref);
bool is_empty_stack (stack_ref);

#endif
