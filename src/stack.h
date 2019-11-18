#ifndef STACK_H
#define STACK_H

#define CAPACITY 500

#include <stdlib.h>

#include "parse.h"

struct stack
{
    struct token **array;
    int capcity;
    int size;
};

struct stack *create_stack(void);
void push_stack(struct stack *stack, struct token *node);
struct token *pop_stack(struct stack *stack);
void free_stack(struct stack *stack);

#endif
