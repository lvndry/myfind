#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "errors.h"

void func_failure(char *message)
{
    errx(1, "myfind: %s", message);
}

void print_error(char *path,  char *error)
{
    fprintf(stderr, "./myfind: %s: %s\n", path, error);
}

void error_exit(enum error_type type, char *error)
{
    switch (type)
    {
    case MISS_ARG:
        fprintf(stderr, "./myfind: Missing argument to `%s'\n", error);
        break;
    case UNKN_PRED:
        fprintf(stderr, "./myfind: Unkown predicate `%s'\n", error);
        break;
    case UNKN_ARG:
        fprintf(stderr, "./myfind: Unknown argument to %s\n", error);
        break;
    case INV_ARG:
        fprintf(stderr, "./myfind: Invalid argument; %s\n", error);
        break;
    case PATH:
        fprintf(stderr, "./myfind: paths must precede expression: %s\n", error);
        break;
    case PAREN:
        fprintf(stderr, "./myfind: invalid expression; %s\n", error);
        break;
    default:
        fprintf(stderr, "./myfind: %s\n", error);
        break;
    }
    exit(EXIT_FAILURE);
}

void exec_error(void)
{
    fprintf(
        stderr,
        "./myfind: Only one instance of {} is supported with -exec...\n"
        );
    exit(EXIT_FAILURE);
}