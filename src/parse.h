#ifndef PARSE_H
#define PARSE_H

#define VALUE_SIZE 100

struct parser
{
    char *value;
    struct token (*func)(char *argv[], int* cursor);
};

enum token_category
{
    OPERATOR, // -a, -o, !, (, )
    TEST, // -name, -type, -perm, -newer, -user, -group
    ACTION, // -exec, -exedir, -delete, -print
};

enum token_type
{
    NONE,

    AND,
    OR,
    NOT,
    PAREN_O,
    PAREN_C,

    NAME,
    TYPE,
    NEWER,
    PERM,
    USER,
    GROUP,

    EXEC,
    EXECDIR,
    EXECPLUS,
    DELETE,
    PRINT
};

struct token
{
    enum token_type type;
    enum token_category category;
    char **value;
};

// operators/operands stack management functions
struct token *create_token(
    enum token_type type,
    enum token_category category,
    char **value
);

struct stack *parse(char *argv[], int start, int end);
int isOperator(const char *op);

#endif