
#ifndef _DS_H
#define _DS_H

#include <stdlib.h>

typedef struct _stack_node {
  void* content;
  struct _stack_node* next;
} stack_node;

typedef struct {
  stack_node* head;
  int size;
} stack;

stack* init_stack();
void push(stack* stk, void* val);
void* pop(stack* stk);
int is_empty(stack *stk);
void free_stack(stack* stk);

#endif
