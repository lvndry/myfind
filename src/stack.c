#include <stdlib.h>
#include <stdio.h>

#include "stack.h"
#include "errors.h"

struct stack *create_stack(void)
{
    struct stack *stack = malloc(sizeof(struct stack));
    if (stack == NULL)
        func_failure("Malloc fail");

    stack->capcity = CAPACITY;
    stack->size = 0;
    stack->array = malloc(sizeof(struct token *) * CAPACITY);

    return stack;
}

void push_stack(struct stack *stack, struct token *token)
{
    if (stack->size < stack->capcity)
    {
        stack->array[stack->size] = token;
        stack->size += 1;
    }
}

// The returned node still need to be freed by the user
struct token *pop_stack(struct stack *stack)
{
    if (stack->size == 0)
        return NULL;

    struct token *token = stack->array[stack->size - 1];
    stack->size -= 1;

    return token;
}

void free_stack(struct stack *stack)
{
    free(stack->array);
    free(stack);
}

void print_stack(struct stack *stack)
{
    for (int i = 0; i < stack->size; i++)
        printf("%d ", stack->array[i]->type);
}
