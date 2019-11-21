#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "errors.h"
#include "memory.h"
#include "parse.h"
#include "parseor.h"
#include "parseand.h"
#include "stack.h"
#include "utils.h"

#define SIZE 100

struct parser parse_table[] = {
    // tests
    {"-name", parse_name},
    {"-type", parse_type},
    {"-newer", parse_newer},
    {"-perm", parse_perm},
    {"-group", parse_group},
    {"-user", parse_user},

    // actions
    {"-print", parse_print},
    {"-delete", parse_delete},
    {"-exec", parse_exec},
    {"-execdir", parse_execdir},

    // operators
    {"-o", parse_or},
    {"or", parse_or},
    {"OR", parse_or},
    {"-a", parse_and},
    {"and", parse_and},
    {"AND", parse_and},
    {"(", parse_oparen},
    {")", parse_cparen},
    {"!", parse_not},
    {"NOT", parse_not},
};

/* TO DELETE */
void print_stacks(struct stack *stack)
{
    struct token *t;

    for (int i = 0; i < stack->size; i++)
    {
        t = stack->array[i];
        if (t->type == AND)
            printf("AND ");
        else if (t->type == OR)
            printf("OR ");
        else if (t->type == NOT)
            printf("! ");
        else if (t->type == PAREN_O)
            printf("( ");
        else if (t->type == PAREN_C)
            printf(") ");
        else if (t->type == PRINT)
            printf("print ");
        else
            printf("%s ", t->value[0]);
    }
}

int getPrecedence(enum token_type type)
{
    if (type == PAREN_O)
        return 1;
    if (type == PAREN_C)
        return 1;
    else if (type == OR)
        return 2;
    else if (type == AND)
        return 3;
    else if (type == NOT)
        return 4;
    return 0; // Should return error
}

int isOperator(const char *op)
{
    if (
        strcmp("-o", op) == 0
        || strcmp("or", op) == 0
        || strcmp("-a", op) == 0
        || strcmp("and", op) == 0
        || strcmp("!", op) == 0
    )
        return 1;
    return 0;
}

struct token *create_token(
    enum token_type type,
    enum token_category category,
    char **value
)
{
    struct token *token = xmalloc(sizeof(struct token));
    token->type = type;
    token->category = category;
    if (value == NULL)
    {
        value = xmalloc(sizeof(char *) * 20);
        if (value == NULL)
            func_failure("malloc fail");
    }

    token->value = value;
    return token;
}

struct stack *parse(char *argv[], int start, int end)
{
    int cursor = start;
    int len = sizeof(parse_table) / sizeof(parse_table[0]);
    bool pushand = false;

    // postfix stack
    struct stack *poststack = create_stack();
    // operators stack
    struct stack *orstack = create_stack();

    while (cursor < end)
    {
        int i = 0;
        for (i = 0; i < len; ++i)
        {
            if (strcmp(parse_table[i].value, argv[cursor]) == 0)
            {
                if (strcmp(argv[cursor], "(") == 0)
                {
                    struct token *token = create_token(PAREN_O, OPERATOR, NULL);
                    push_stack(orstack, token);
                }
                else if (strcmp(argv[cursor], ")") == 0)
                {
                    while (
                        orstack->size > 0
                        && orstack->array[orstack->size - 1]->type != PAREN_O
                    )
                        push_stack(poststack, pop_stack(orstack));
                    struct token *tok = pop_stack(orstack);
                    free(tok->value);
                    free(tok);
                }
                else if (!isOperator(argv[cursor]))
                {
                    if (pushand)
                    {
                        struct token *and = create_token(AND, OPERATOR, NULL);
                        push_stack(orstack, and);
                    }

                    struct token tok = parse_table[i].func(argv, &cursor);
                    struct token *token = create_token(
                        tok.type,
                        tok.category,
                        tok.value
                    );
                    push_stack(poststack, token);
                    pushand = true;
                }
                else
                {
                    pushand = false;
                    struct token tok = parse_table[i].func(argv, &cursor);
                    struct token *token = create_token(
                        tok.type,
                        tok.category,
                        tok.value
                    );
                    while (
                        orstack->size - 1 >= 0 &&
                        getPrecedence(orstack->array[orstack->size - 1]->type)
                        >= getPrecedence(token->type)
                    )
                        push_stack(poststack, pop_stack(orstack));
                    push_stack(orstack, token);
                }
                break;
            }
        }
        if (i >= len - 1)
        {
            destroy_stack(orstack);
            destroy_stack(poststack);
            error_exit(UNKN_PRED, argv[cursor]);
        }
        cursor++;
    }
    while (orstack->size > 0)
        push_stack(poststack, pop_stack(orstack));

    destroy_stack(orstack);
    poststack->array[poststack->size] = NULL;

    return poststack;
}
