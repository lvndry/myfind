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

void parse_error(enum error_type type)
{
    switch (type)
    {
    case PARSE:
        func_failure("./myfind: Error while parsing");
        break;
    case CMD_MALFORMED:
        func_failure("./myfind: Syntaxt error");
        break;
    default:
        func_failure("Something wrong happened");
        break;
    }
}
