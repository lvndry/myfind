#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "parse.h"
#include "utils.h"
#include "errors.h"

#define SIZE 50

struct parser parse_table[] = {

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
};

struct token operators[SIZE];
struct token operands[SIZE];
struct token postfix[SIZE];

int topo = 0;
int topan = 0;
int toppost = 0;

/* TO DELETE */
void print_stacks()
{
    struct token t;

    for (int i = 0; i < toppost; i++)
    {
        t = postfix[i];
        if (t.type == AND)
            printf("AND ");
        else if (t.type == OR)
            printf("OR ");
        else if (t.type == NOT)
            printf("! ");
        else if (t.type == PAREN_O)
            printf("( ");
        else if (t.type == PAREN_C)
            printf(") ");
        else if (t.type == PRINT)
            printf("print ");
        else
            printf("%s ", t.value[0]);
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

void push_operand(struct token token)
{
    operands[topan++] = token;
}

struct token pop_operand()
{
    return operands[--topan];
}

void push_operator(struct token token)
{
    operators[topo++] = token;
}

struct token pop_operator()
{
    return operators[--topo];
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

struct token *parse(char *argv[], int start, int end)
{
    int cursor = start;
    int len = sizeof(parse_table) / sizeof(parse_table[0]);
    bool pushand = false;

    while (cursor < end)
    {
        for (int i = 0; i < len; i++)
        {
            if (strcmp(parse_table[i].value, argv[cursor]) == 0)
            {
                if (strcmp(argv[cursor], "(") == 0)
                {
                    struct token token = { PAREN_O, OPERATOR, NULL };
                    push_operator(token);
                }
                else if (strcmp(argv[cursor], ")") == 0)
                {
                    while (operators[topo - 1].type != PAREN_O)
                        postfix[toppost++] = pop_operator();
                    pop_operator(); // pop last "("
                }
                else if (!isOperator(argv[cursor]))
                {
                    if (pushand)
                    {
                        struct token and = { AND, OPERATOR, NULL };
                        push_operator(and);
                    }

                    struct token token = parse_table[i].func(argv, &cursor);
                    postfix[toppost++] = token;
                    pushand = true;
                }
                else
                {
                    pushand = false;
                    struct token token = parse_table[i].func(argv, &cursor);
                    while (topo - 1 >= 0 && getPrecedence(operators[topo - 1].type) >= getPrecedence(token.type))
                        postfix[toppost++] = pop_operator();
                    push_operator(token);
                }
                break;
            }
        }
        cursor++;
    }
    while (topo > 0)
        postfix[toppost++] = pop_operator();

    return postfix;
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
    *cursor += 1;
    if (argv[*cursor] == NULL)
        parse_error(CMD_MALFORMED);

    char **value = malloc(sizeof(argv[*cursor]));
    value[0] = argv[*cursor];
    struct token token = { NAME, TEST, value };
    return token;
}

struct token parse_type(char *argv[], int *cursor)
{
    *cursor += 1;
    if (argv[*cursor] == NULL)
        parse_error(CMD_MALFORMED);

    char **value = malloc(sizeof(argv[*cursor]));
    value[0] = argv[*cursor];
    struct token token = { TYPE, TEST, value };
    return token;
}

struct token parse_newer(char *argv[], int *cursor)
{
    *cursor += 1;
    if (argv[*cursor] == NULL)
        parse_error(CMD_MALFORMED);

    char **value = malloc(sizeof(argv[*cursor]));
    value[0] = argv[*cursor];
    struct token token = { NEWER, TEST, value };
    return token;
}

struct token parse_perm(char *argv[], int *cursor)
{
    *cursor += 1;
    if (argv[*cursor] == NULL)
        parse_error(CMD_MALFORMED);

    char **value = malloc(sizeof(argv[*cursor]));
    value[0] = argv[*cursor];
    struct token token = { PERM, TEST, value };
    return token;
}

struct token parse_group(char *argv[], int *cursor)
{
    *cursor += 1;
    if (argv[*cursor] == NULL)
        parse_error(CMD_MALFORMED);

    char **value = malloc(sizeof(argv[*cursor]));
    value[0] = argv[*cursor];
    struct token token = { GROUP, TEST, value };
    return token;
}

struct token parse_user(char *argv[], int *cursor)
{
    *cursor += 1;
    if (argv[*cursor] == NULL)
        parse_error(CMD_MALFORMED);

    char **value = malloc(sizeof(argv[*cursor]));
    value[0] = argv[*cursor];
    struct token token = { USER, TEST, value };
    return token;
}

// actions functions

struct token parse_print(char *argv[], int* cursor)
{
    UNUSED(argv);
    UNUSED(cursor);

    char **value = malloc(sizeof(2));
    value[0] = "ignore";

    for (int i = 0; i < toppost; i++)
    {
        if (postfix[i].type == PRINT)
        {
            value[0] = NULL;
            break;
        }
    }

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
    char **value = malloc(VALUE_SIZE * sizeof(char));
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

    struct token token = { EXEC, ACTION, value };
    return token;
}

struct token parse_execdir(char *argv[], int *cursor)
{
    char **value = malloc(VALUE_SIZE * sizeof(char));
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

// struct token parse_execplus(char *argv[], int *cursor)
