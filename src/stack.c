#include <stdlib.h>

#include "stack.h"
#include "errors.h"

struct stack *create(void *elt)
{
    if (elt == NULL)
        return NULL;

    struct stack *stack = malloc(sizeof(struct stack));
    struct node *node = malloc(sizeof(struct node));

    if (stack == NULL || node == NULL)
        func_failure("Malloc fail");

    node->data = elt;
    stack->tail = node;
    stack->tail->next = NULL;
    stack->size = 1;
}

void push(struct stack *stack, void *elt)
{
    struct node *node = malloc(sizeof(struct node));
    if (node == NULL)
        func_failure("Malloc fail");

    node->data = node;
    node->next = NULL;
    node->prev = stack->tail;
    stack->tail->next = node;
    stack->tail = node;
    stack->size += 1;
}

// The returned node still need to be freed by the user
struct node *pop(struct stack *stack)
{
    if (stack->size == 0)
        return NULL;

    struct node *tail = stack->tail;
    stack->tail = stack->tail->prev;
    stack->size -= 1;

    return tail;
}
