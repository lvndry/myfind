#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "errors.h"

void func_failure(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void parse_error(enum error_type type, char *error)
{
    switch (type)
    {
    case PARSE:
        func_failure("./myfind: Error while parsing");
        break;
    case MISS_ARG:
        fprintf(stderr, "./myfind: Missing argument to `%s", error);
        exit(EXIT_FAILURE);
        break;
    case UNKN_PRED:
        fprintf(stderr, "./myfind: Unkown predicate `%s", error);
        exit(EXIT_FAILURE);
        break;
    default:
        func_failure("Something wrong happened");
        break;
    }
}

void print_error(char *path,  char *error)
{
    fprintf(stderr, "./myfind: %s: %s\n", path, error);
}
