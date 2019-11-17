#ifndef STACK_H
#define STACK_H

#include <stdlib.h>

struct node
{
    void *data;
    struct node *next;
    struct node* prev;
};

struct stack
{
    struct node *tail;
    size_t size;
};

struct stack *init(void);
void push(struct stack* stack, void *node);
struct node *pop(struct stack *stack);

#endif
