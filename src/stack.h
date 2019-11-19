#ifndef STACK_H
#define STACK_H

#define CAPACITY 500

#include <stdlib.h>

#include "parse.h"
#include "ast.h"

struct stack
{
    struct token **array;
    int capcity;
    int size;
};

struct stack_ast
{
    struct ast **array;
    int capcity;
    int size;
};

// stack functions
struct stack *create_stack(void);
void push_stack(struct stack *stack, struct token *node);
struct token *pop_stack(struct stack *stack);
void free_stack(struct stack *stack);
void destroy_stack(struct stack *stack);

// stack_ast functions
struct stack_ast *create_astack(void);
void push_astack(struct stack_ast *stack, struct ast *node);
struct ast *pop_astack(struct stack_ast *stack);
void free_astack(struct stack_ast *stack);
void destroy_astack(struct stack_ast *stack);

#endif