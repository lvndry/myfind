#include <stdlib.h>

#include "memory.h"
#include "parse.h"
#include "errors.h"

// tests functions
struct token parse_name(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        error_exit(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = xmalloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { NAME, TEST, value };
    return token;
}

struct token parse_type(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        error_exit(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = xmalloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { TYPE, TEST, value };
    return token;
}

struct token parse_newer(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        error_exit(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = xmalloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { NEWER, TEST, value };
    return token;
}

struct token parse_perm(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        error_exit(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = xmalloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { PERM, TEST, value };
    return token;
}

struct token parse_group(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        error_exit(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = xmalloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { GROUP, TEST, value };
    return token;
}

struct token parse_user(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] == NULL)
        error_exit(MISS_ARG, argv[*cursor]);

    *cursor += 1;
    char **value = xmalloc(sizeof(char *) * 2);
    value[0] = argv[*cursor];
    struct token token = { USER, TEST, value };
    return token;
}

// actions functions
struct token parse_print(char *argv[], int *cursor)
{
    if (argv[*cursor + 1] != NULL && argv[*cursor + 1][0] != '-')
        error_exit(PATH, argv[*cursor + 1]);
    char **value = xmalloc(sizeof(char *) * 2);
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
    char **value = xmalloc(VALUE_SIZE * sizeof(char *) * 500);
    int count = 0;
    int i;

    *cursor += 1;

    for (i = 0; argv[*cursor + i] != NULL && (argv[*cursor + i][0] != ';')
        && (argv[*cursor + i][0] != '+'); i++)
    {
        if (argv[*cursor + i][0] == '{' && argv[*cursor + i][1] == '}')
            count++;
        value[i] = argv[*cursor + i];
    }

    if (i == 0)
        error_exit(INV_ARG, "-exec");

    if (argv[*cursor + i] == NULL)
        error_exit(MISS_ARG, "-exec");

    struct token token = { EXEC, ACTION, value };
    if (argv[*cursor + i][0] == '+')
    {
        if (argv[*cursor + i - 1][0] != '{' || argv[*cursor + i - 1][1] != '}')
            error_exit(MISS_ARG, "-exec+");

        if (count > 1)
            exec_error();

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
    char **value = xmalloc(VALUE_SIZE * sizeof(char *) * 500);

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