#ifndef PARSE_H
#define PARSE_H

struct parser {
    char *value;
    struct token (*func)(char *argv[], int* cursor);
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
    PAREN_C,

    EXEC,
    EXECDIR,
    EXECPLUS,
    DELETE,
    PRINT,
    NONE
};

struct token {
    enum token_type type;
    char *value;
};

void parse(char *argv[], int start, int end);
int isOperand(const char *op);

// token parsing functions
struct token parse_name(char *argv[], int* cursor);
struct token parse_oparen(char *argv[], int* cursor);
struct token parse_cparen(char *argv[], int *cursor);
struct token parse_print(char *argv[], int* cursor);
struct token parse_or(char *argv[], int *cursor);
struct token parse_and(char *argv[], int *cursor);
struct token parse_not(char *argv[], int *cursor);

// Stack functions
void push_operator(struct token token);
struct token pop_operator();
void push_operand(struct token token);
struct token pop_operand();

#endif