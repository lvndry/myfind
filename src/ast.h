#ifndef AST_H
#define AST_H

#include "parse.h"
#include "stack.h"

struct params
{
  char *pathname;
  char *filename;
  char **argv;
  char **execvalue;
  int shouldprint;
};

struct ast
{
    struct token *token;
    struct ast *left;
    struct ast *right;
};

/*
*  int (*function)(char *data, char *filename):
*  char *argv[]: List of arguments for the expression
*  char *data: data that we want to compare or act on
*  char *filename: data that we compare to
*/
struct expression
{
    enum token_type type;
    int (*function)(struct params *params);
};

// ast functions
struct ast *create_node(struct token *token);
struct ast *constructTree(struct stack *postfix);
int isParent(enum token_type type);
void free_ast(struct ast *root);
int evaluate(struct ast* ast, struct params *params);

#endif