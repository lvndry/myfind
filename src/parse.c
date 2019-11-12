#include <stdio.h>
#include <string.h>

#include "parse.h"

#define SIZE 50
#define UNUSED(x) (void)(x)

struct expression parse_table[] = {
    {"-name", parse_name},
    {"(", parse_oparen},
};

struct token tokens[SIZE];
int top = -1;

void push(struct token token)
{
    tokens[++top] = token;
}

struct token pop()
{
    return tokens[top--];
}

int isOperand(const char *op)
{
    if (strcmp("-o", op) == 0 || strcmp("-a", op) == 0)
        return 1;
    return 0;
}

void parse(char *argv[], int start, int end)
{
    int cursor = start;
    int len = sizeof(parse_table) / sizeof(parse_table[0]);

    while (cursor < end)
    {
        for (int i = 0; i < len; ++i)
        {
            if (strcmp(parse_table[i].value, argv[cursor]) == 0)
            {
                parse_table[i].func(argv, &cursor);
            }
        }
        cursor++;
    }
}

int parse_name(char *argv[], int* cursor)
{
    printf("We got a %s expression and parameter is %s\n", argv[*cursor], argv[*cursor + 1]);
    struct token token = { NAME, argv[*cursor + 1] };
    push(token);
    *cursor += 1;

    return 1;
}

int parse_oparen(char *argv[], int *cursor)
{
    UNUSED(argv);
    UNUSED(cursor);

    struct token token = { PAREN_O, NULL };
    push(token);

    return 1;
}
