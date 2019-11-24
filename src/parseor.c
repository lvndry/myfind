#include <string.h>
#include <stdlib.h>

#include "memory.h"
#include "parse.h"
#include "errors.h"

// Operators functions
int isAllowedAfter(const char *op)
{
    if (
        strcmp(op, "-o") == 0
        || strcmp(op, "-a") == 0
        || ((op[0] != '-' && op[0] != '!')
        && (op[0] != '-' && op[0] != '('))
    )
        return 0;
    return 1;
}

struct token parse_and(char *argv[], int *cursor)
{
    if (*cursor == 1)
    {
        error_exit(
            INV_ARG,
            "you have used a binary operator '-a' with nothing before it."
            );
    }
    if (argv[*cursor + 1] == NULL || !isAllowedAfter(argv[*cursor + 1]))
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
    if (argv[*cursor + 1] == NULL || !isAllowedAfter(argv[*cursor + 1]))
        error_exit(PATH, argv[*cursor]);

    struct token token = { OR, OPERATOR, NULL };
    return token;
}

struct token parse_oparen(char *argv[], int *cursor)
{
    if (argv[*cursor + 1][0] == ')' || !isAllowedAfter(argv[*cursor + 1]))
        error_exit(MISS_ARG, "(");
    struct token token = { PAREN_O, OPERATOR, NULL };
    return token;
}

struct token parse_cparen(char *argv[], int *cursor)
{
    if (argv[*cursor - 1][0] == '(')
        error_exit(MISS_ARG, ")");
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
