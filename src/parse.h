#ifndef PARSE_H
#define PARSE_H

#define VALUE_SIZE 100

struct parser {
    char *value;
    struct token (*func)(char *argv[], int* cursor);
};

enum token_category
{
    OPERATOR, // -a, -o, !, (, )
    TEST, // -name, -type, -perm, -newer, -user, -group
    ACTION, // -exec. -exedir, -delete, -print
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

struct token {
    enum token_type type;
    enum token_category category;
    char **value;
};

struct token *parse(char *argv[], int start, int end);
int isOperand(const char *op);

// token parsing functions
struct token parse_name(char *argv[], int* cursor);
struct token parse_oparen(char *argv[], int* cursor);
struct token parse_cparen(char *argv[], int *cursor);
struct token parse_print(char *argv[], int* cursor);
struct token parse_or(char *argv[], int *cursor);
struct token parse_and(char *argv[], int *cursor);
struct token parse_not(char *argv[], int *cursor);
struct token parse_type(char *argv[], int *cursor);
struct token parse_newer(char *argv[], int *cursor);
struct token parse_perm(char *argv[], int *cursor);
struct token parse_group(char *argv[], int *cursor);
struct token parse_user(char *argv[], int *cursor);
struct token parse_delete(char *argv[], int *cursor);
struct token parse_exec(char *argv[], int *cursor);
struct token parse_execdir(char *argv[], int *cursor);

// operators/operands stack management functions
void push_operator(struct token token);
struct token pop_operator();
void push_operand(struct token token);
struct token pop_operand();

#endif
