#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "errors.h"
#include "parse.h"
#include "stack.h"
#include "utils.h"

#define SIZE 50

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
    struct token *token = malloc(sizeof(struct token));
    token->type = type;
    token->category = category;
    if (value == NULL)
    {
        value = malloc(sizeof(char *) * 20);
        if (value == NULL)
            func_failure("Malloc fail");
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
        for (i = 0; i < len; i++)
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
                    pop_stack(orstack);
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
        if (i == len - 1)
        {
            free_stack(orstack);
            free_stack(poststack);
            parse_error(UNKN_PRED, argv[cursor]);
        }
        cursor++;
    }
    while (orstack->size > 0)
        push_stack(poststack, pop_stack(orstack));

    free_stack(orstack);
    poststack->array[poststack->size] = NULL;
    return poststack;
}

// Operators functions

struct token parse_and(char *argv[], int *cursor)
{
    UNUSED(argv);
    UNUSED(cursor);

    struct token token = { AND, OPERATOR, NULL };
    return token;
}

struct token parse_or(char *argv[], int *cursor)
{
    UNUSED(argv);
    UNUSED(cursor);

    struct token token = { OR, OPERATOR, NULL };
    return token;
}


struct token parse_oparen(char *argv[], int *cursor)
{
    UNUSED(argv);
    UNUSED(cursor);

    struct token token = { PAREN_O, OPERATOR, NULL };
    return token;
}

struct token parse_cparen(char *argv[], int *cursor)
{
    UNUSED(argv);
    UNUSED(cursor);

    struct token token = { PAREN_C, OPERATOR, NULL };
    return token;
}

struct token parse_not(char *argv[], int *cursor)
{
    UNUSED(argv);
    UNUSED(cursor);

    struct token token = { NOT, OPERATOR, NULL };
    return token;
}

// tests functions

struct token parse_name(char *argv[], int* cursor)
{
    if (argv[*cursor + 1] == NULL)
        parse_error(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = malloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { NAME, TEST, value };
    return token;
}

struct token parse_type(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        parse_error(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = malloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { TYPE, TEST, value };
    return token;
}

struct token parse_newer(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        parse_error(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = malloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { NEWER, TEST, value };
    return token;
}

struct token parse_perm(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        parse_error(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = malloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { PERM, TEST, value };
    return token;
}

struct token parse_group(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        parse_error(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = malloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { GROUP, TEST, value };
    return token;
}

struct token parse_user(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        parse_error(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = malloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { USER, TEST, value };
    return token;
}

// actions functions

struct token parse_print(char *argv[], int* cursor)
{
    UNUSED(argv);
    UNUSED(cursor);

    char **value = malloc(sizeof(char *) * 2);
    value[0] = NULL;

    struct token token = { PRINT, ACTION, value };
    return token;
}

struct token parse_delete(char *argv[], int *cursor)
{
    UNUSED(argv);
    UNUSED(cursor);

    struct token token = { DELETE, ACTION, NULL };
    return token;
}

struct token parse_exec(char *argv[], int *cursor)
{
    char **value = malloc(VALUE_SIZE * sizeof(char *));
    if (value == NULL)
       func_failure("malloc fail");

    int i;
    *cursor += 1;

    for (
        i = 0;
        argv[*cursor + i] != NULL
        && (argv[*cursor + i][0] != ';' || (argv[*cursor + i][1] != '\0'))
        && (argv[*cursor + i][0] != '+' || (argv[*cursor + i][1] != '\0'));
        i++
    )
    {
        value[i] = argv[*cursor + i];
    }

    struct token token = { EXEC, ACTION, value };
    if (argv[*cursor + i][0] == '+')
    {
        token.type = EXECPLUS;
        i++;
    }

    // TODO: if argv[*cursor] == NULL throw error
    value[i] = NULL;

    *cursor += i;
    token.value = value;

    return token;
}

struct token parse_execdir(char *argv[], int *cursor)
{
    char **value = malloc(VALUE_SIZE * sizeof(char *));
    if (value == NULL)
       func_failure("malloc fail");

    int i;
    *cursor += 1;

    for (
        i = 0;
        argv[*cursor + i] != NULL
        && (argv[*cursor + i][0] != ';' || (argv[*cursor + i][1] != '\0'));
        i++
    )
    {
        value[i] = argv[*cursor + i];
    }

    // TODO: if argv[*cursor] == NULL throw error
    value[i] = NULL;

    *cursor += i;

    struct token token = { EXECDIR, ACTION, value };
    return token;
}
