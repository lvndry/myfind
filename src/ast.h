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

struct params
{
  char *pathname;
  char *filename;
  char **value;
  char **execvalue;
  int shouldprint;
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

struct ast *create_node(struct token token);
void push_node(struct ast *node);
struct ast *pop_node();
void remove_node(struct ast *ast);
struct ast *constructTree(struct token postfix[]);
int evaluate(struct ast* ast, struct params *params);
void free_ast(struct ast *root);

// Evaluation functions
int is_newer(struct params *params);
int print(struct params *params);
int group_own(struct params *params);
int user_own(struct params *params);
int rm(struct params *params);
int has_name(struct params *params);
int has_type(struct params *params);
int has_perm(struct params *params);
int execute(struct params *params);
int executedir(struct params *params);
int executeplus(struct params *params);

extern int shouldprint;

#endif
