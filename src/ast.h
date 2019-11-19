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

struct stack_ast
{
    struct ast **array;
    int capcity;
    int size;
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

// ast_stack functions
struct stack_ast *create_astack(void);
void push_astack(struct stack_ast *stack, struct ast *node);
struct ast *pop_astack(struct stack_ast *stack);
void free_astack(struct stack_ast *stack);

// evaluation functions
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

#endif
