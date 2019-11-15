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
    // Should be const char*
    int (*function)(char *argv[], char *data, const char *filename);
};

struct ast *create_node(struct token token);
void push_node(struct ast *node);
struct ast *pop_node();
void remove_node(struct ast *ast);
struct ast *constructTree(struct token postfix[]);
int evaluate(struct ast* ast, char *pathname, char *filename);

// Evaluation functions
int is_newer(char *argv[], char *timestamp, const char *filename);
int print(char *argv[],  char *isFolder, const char *filename);
int group_own(char *argv[], char *gid, const char *filename);
int user_own(char *argv[], char *uid, const char *filename);
int rm(char *argv[], char *placeholder, const char *filename);
int has_name(char *argv[], char *name, const char *filename);
int has_type(char *argv[], char *pathname, const char *filename);
int has_perm(char *argv[], char *pathname, const char *filename);

#endif
