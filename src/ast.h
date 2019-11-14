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

struct ast *create_node(struct token token);
void push_node(struct ast *node);
struct ast *pop_node();
void remove_node(struct ast *ast);
struct ast *constructTree(struct token postfix[]);
int eval(struct ast* expresssion);

int is_newer(char *path, unsigned int *timestamp);
int print(char *path,  unsigned int *isFolder);
int group_own(char *path, unsigned int *gid);
int user_own(char *path, unsigned int *uid);
int rm(char *path, unsigned int *placeholder);

#endif
