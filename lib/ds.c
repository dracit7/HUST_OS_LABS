
#include "ds.h"

// init_stack returns a stack.
stack* init_stack() {

  // Malloc the stack
  stack* stk = (stack *)malloc(sizeof(stack));
  if (stk == NULL) {
    return NULL;
  }

  // Malloc the head node
  stk->size = 0;
  stk->head = (stack_node *)malloc(sizeof(stack_node));
  stk->head->content = NULL;
  stk->head->next = NULL;

  return stk;
}

// push a new value into stack.
void push(stack* stk, void* val) {

  stack_node* tmp = stk->head->next;

  stk->size += 1;
  stk->head->next = (stack_node *)malloc(sizeof(stack_node));
  stk->head->next->content = val;
  stk->head->next->next = tmp;

  return;
}

// pop a value out of stack.
void* pop(stack* stk) {

  stack_node* tmp = stk->head->next;
  void* val = tmp->content;

  stk->size -= 1;
  stk->head->next = stk->head->next->next;
  free(tmp);

  return val;
}

int is_empty(stack *stk) {
  return stk->size == 0;
}

// free the stack.
void free_stack(stack* stk) {

  stack_node* tmp = stk->head->next;

  while (tmp != NULL) {
    stk->head->next = tmp->next;
    free(tmp);
    tmp = stk->head->next;
  }

  free(stk);
}