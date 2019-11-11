#ifndef AST_H
#define AST_H

#define FUN_LENGTH 5

enum node_type
{
    OR,
    AND,
    NOT,
    PARENTHESES,
    OPERATOR,
    ARG
};

struct token
{
    enum node_type type;
    char *value;
};

struct node
{
    enum node_type type;
    struct node *left;
    struct node *right;
    char *value;
};

struct expression
{
    enum node_type type;
    char *name;
    int (*function)(char *path, void* data);
};

struct expression expressions[FUN_LENGTH] = {
    {
        .name = "newer",
        .function = is_newer,
    },
    {
        .name = "print",
        .function = print,
    },
    {
        .name = "group",
        .function = group_own,
    },
    {
        .name = "user",
        .function = user_own,
    },
    {
        .name = "delete",
        .function = rm,
    },
};

struct token
{
    enum node_type type;
    char *value;
    int (*parse_function)(char *argv[], int *arg_ptr);
};

struct node *init(void* value, enum node_type);
void remove_node(struct node *node);
int eval(struct node* expresssion);
int is_newer(char *path, unsigned int *timestamp);
int print(char *path,  unsigned int *isFolder);
int group_own(char *path, unsigned int *gid);
int user_own(char *path, unsigned int *uid);
int rm(char *path, unsigned int *placeholder);

#endif
