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

/*
*  int (*function)(char *argv[], char *data, char *filename):
*  char *argv[]: List of arguments for the expression
*  char *data: data that we want to compare or act on
*  char *filename: data that we compare to
*/
struct expression
{
    enum token_type type;
    int (*function)(char *argv[], char *data, char *filename);
};

struct ast *create_node(struct token token);
void push_node(struct ast *node);
struct ast *pop_node();
void remove_node(struct ast *ast);
struct ast *constructTree(struct token postfix[]);
int evaluate(struct ast* ast, char *pathname, char *filename);
void free_ast(struct ast *root);

// Evaluation functions
int is_newer(char *argv[], char *timestamp, char *filename);
int print(char *argv[],  char *isFolder, char *filename);
int group_own(char *argv[], char *gid, char *filename);
int user_own(char *argv[], char *uid, char *filename);
int rm(char *argv[], char *placeholder, char *filename);
int has_name(char *argv[], char *name, char *filename);
int has_type(char *argv[], char *pathname, char *filename);
int has_perm(char *argv[], char *pathname, char *filename);
int execute(char *argv[], char *pathnmae, char *filename);
int executedir(char *argv[], char *pathnmae, char *filename);

#endif
