#ifndef PARSE_H
#define PARSE_H

struct expression {
    char *value;
    int (*func)(char *argv[], int* cursor);
};

enum token_type
{
    NAME,
    TYPE,
    NEWER,
    PERM,
    USER,
    GROUP,

    AND,
    OR,
    NOT,
    PAREN_O,
    PAREN_P,

    EXEC,
    EXECDIR,
    EXECPLUS,
    DELETE,
    PRINT
};

struct token {
    enum token_type type;
    char *value;
};

void parse(char *argv[], int start, int end);
int isOperand(const char *op);

// token parsing functions
int parse_name(char *argv[], int* cursor);
int parse_oparen(char *argv[], int* cursor);

#endif
