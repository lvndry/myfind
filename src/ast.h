#ifndef AST_H
#define AST_H

#define FUN_LENGTH 5

#include "parse.h"

struct ast
{
    struct token token;
    struct ast *left;
    struct ast *right;
};

struct expression
{
    enum token_type type;
    int (*function)(char *path, unsigned int * data);
};

struct ast *create(struct token token);
struct ast *add(struct ast *ast, struct token);
void remove_node(struct ast *ast);
int eval(struct ast* expresssion);
int is_newer(char *path, unsigned int *timestamp);
int print(char *path,  unsigned int *isFolder);
int group_own(char *path, unsigned int *gid);
int user_own(char *path, unsigned int *uid);
int rm(char *path, unsigned int *placeholder);

struct expression expressions[] = {
    {
        .type = NEWER,
        .function = is_newer,
    },
    {
        .type = PRINT,
        .function = print,
    },
    {
        .type = GROUP,
        .function = group_own,
    },
    {
        .type = USER,
        .function = user_own,
    },
    {
        .type = DELETE,
        .function = rm,
    },
};

#endif
