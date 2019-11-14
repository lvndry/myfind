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
    int (*function)(char *argv[], char *data);
};

struct ast *create_node(struct token token);
void push_node(struct ast *node);
struct ast *pop_node();
void remove_node(struct ast *ast);
struct ast *constructTree(struct token postfix[]);
int evaluate(struct ast* expresssion);

// Evaluation functions
int is_newer(char *argv[], char *timestamp);
int print(char *argv[],  char *isFolder);
int group_own(char *argv[], char *gid);
int user_own(char *argv[], char *uid);
int rm(char *argv[], char *placeholder);
int has_name(char *argv[], char *name);

#endif
