#include <stdlib.h>
#include <stdio.h>

#include "errors.h"
#include "memory.h"
#include "stack.h"

struct stack *create_stack(void)
{
    struct stack *stack = xmalloc(sizeof(struct stack));
    if (stack == NULL)
        func_failure("malloc fail");

    stack->capcity = CAPACITY;
    stack->size = 0;
    stack->array = xmalloc(sizeof(struct token *) * CAPACITY);

    return stack;
}

void push_stack(struct stack *stack, struct token *token)
{
    if (stack->size == stack->capcity)
    {
        stack->capcity *= 2;
        stack->array = realloc(stack->array, stack->capcity);
    }
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

void destroy_stack(struct stack *stack)
{
    for (int i = 0; i < stack->size; i++)
    {
        free(stack->array[i]->value);
        free(stack->array[i]);
    }

    free_stack(stack);
    stack = NULL;

    return;
}

void print_stack(struct stack *stack)
{
    for (int i = 0; i < stack->size; i++)
        printf("%d ", stack->array[i]->type);
}

void destroy_astack(struct stack_ast *stack)
{
    int i = 0;

    while (stack->array[i])
    {
        free(stack->array[i]->token);
        free(stack->array[i]);
        stack->array[i] = NULL;
        i++;
    }

    free_astack(stack);
    stack = NULL;

    return;
}

struct stack_ast *create_astack(void)
{
    struct stack_ast *stack = xmalloc(sizeof(struct stack_ast));

    if (stack == NULL)
        func_failure("malloc fail");

    stack->capcity = CAPACITY;
    stack->size = 0;
    stack->array = xmalloc(sizeof(struct ast) * CAPACITY);

    return stack;
}

void push_astack(struct stack_ast *stack, struct ast *node)
{
    if (stack->size < stack->capcity)
    {
        stack->array[stack->size] = node;
        stack->size += 1;
    }
}

// The returned node still need to be freed by the user
struct ast *pop_astack(struct stack_ast *stack)
{
    if (stack->size == 0)
        return NULL;

    struct ast *node = stack->array[stack->size - 1];
    stack->size -= 1;

    return node;
}

void free_astack(struct stack_ast *stack)
{
    free(stack->array);
    free(stack);
}