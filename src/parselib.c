#include <stdlib.h>

#include "parse.h"
#include "errors.h"

// Operators functions

// Still need to handle error like: find -o, -a
struct token parse_and(char *argv[], int *cursor)
{
    if (*cursor == 1)
    {
        error_exit(
            INV_ARG,
            "you have used a binary operator '-o' with nothing before it."
        );
    }
    if (argv[*cursor + 1] == NULL || argv[*cursor + 1][0] != '-')
        error_exit(PATH, argv[*cursor]);
    struct token token = { AND, OPERATOR, NULL };
    return token;
}

struct token parse_or(char *argv[], int *cursor)
{
    if (*cursor == 1)
    {
        error_exit(
            INV_ARG,
            "you have used a binary operator '-o' with nothing before it."
        );
    }
    if (argv[*cursor + 1] == NULL || argv[*cursor + 1][0] != '-')
        error_exit(PATH, argv[*cursor]);

    struct token token = { OR, OPERATOR, NULL };
    return token;
}

struct token parse_oparen(char *argv[], int *cursor)
{
    argv = argv;
    cursor = cursor;

    struct token token = { PAREN_O, OPERATOR, NULL };
    return token;
}

struct token parse_cparen(char *argv[], int *cursor)
{
    argv = argv;
    cursor = cursor;

    struct token token = { PAREN_C, OPERATOR, NULL };
    return token;
}

struct token parse_not(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        error_exit(MISS_ARG, argv[*cursor]);

    struct token token = { NOT, OPERATOR, NULL };
    return token;
}

// tests functions

struct token parse_name(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        error_exit(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = malloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { NAME, TEST, value };
    return token;
}

struct token parse_type(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        error_exit(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = malloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { TYPE, TEST, value };
    return token;
}

struct token parse_newer(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        error_exit(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = malloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { NEWER, TEST, value };
    return token;
}

struct token parse_perm(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        error_exit(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = malloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { PERM, TEST, value };
    return token;
}

struct token parse_group(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        error_exit(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = malloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { GROUP, TEST, value };
    return token;
}

struct token parse_user(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        error_exit(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = malloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { USER, TEST, value };
    return token;
}

// actions functions

struct token parse_print(char *argv[], int* cursor)
{
    if (argv[*cursor + 1] != NULL && argv[*cursor + 1][0] != '-')
        error_exit(PATH, argv[*cursor + 1]);
    char **value = malloc(sizeof(char *) * 2);
    value[0] = NULL;

    struct token token = { PRINT, ACTION, value };
    return token;
}

struct token parse_delete(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] != NULL && argv[*cursor + 1][0] != '-')
        error_exit(PATH, argv[*cursor + 1]);
    struct token token = { DELETE, ACTION, NULL };
    return token;
}

struct token parse_exec(char *argv[], int *cursor)
{
    char **value = malloc(VALUE_SIZE * sizeof(char *) * 500);
    if (value == NULL)
       func_failure("malloc fail");

    int i;
    *cursor += 1;

    for (
        i = 0;
        argv[*cursor + i] != NULL
        && (argv[*cursor + i][0] != ';')
        && (argv[*cursor + i][0] != '+');
        i++
    )
    {
        value[i] = argv[*cursor + i];
    }

    if (i == 0)
        error_exit(INV_ARG, "-exec");

    if (argv[*cursor + i] == NULL)
        error_exit(MISS_ARG, "-exec");

    struct token token = { EXEC, ACTION, value };
    if (argv[*cursor + i][0] == '+')
    {
        token.type = EXECPLUS;
        i++;
    }

    value[i] = NULL;

    *cursor += i;
    token.value = value;

    return token;
}

struct token parse_execdir(char *argv[], int *cursor)
{
    char **value = malloc(VALUE_SIZE * sizeof(char *) * 500);
    if (value == NULL)
       func_failure("malloc fail");

    int i;
    *cursor += 1;

    for (i = 0; argv[*cursor + i] != NULL && (argv[*cursor + i][0] != ';'); i++)
    {
        value[i] = argv[*cursor + i];
    }

    if (i == 0)
        error_exit(INV_ARG, "-execdir");

    if (argv[*cursor + i] == NULL)
        error_exit(MISS_ARG, "-execdir");

    value[i] = NULL;

    *cursor += i;

    struct token token = { EXECDIR, ACTION, value };
    return token;
}