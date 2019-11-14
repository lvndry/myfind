#define _GNU_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fnmatch.h>

#include "ast.h"

#define TREE_SIZE 50

struct ast *tree[TREE_SIZE] = { NULL };
int treetop = 0;

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

void push_node(struct ast *node)
{
    tree[treetop++] = node;
}

struct ast *pop_node()
{
    return tree[--treetop];
}

struct ast *create_node(struct token token)
{
    struct ast *ast = malloc(sizeof(struct ast));

    ast->right = NULL;
    ast->left = NULL;
    ast->token = token;

    return ast;
}

int isParent(enum token_type type)
{
    if (
        type == OR
        || type == AND
        || type == NOT
        || type == NONE
    )
        return 1;
    return 0;
}

// to delete
void inorder(struct ast *t)
{
    if (t)
    {
        inorder(t->left);
        printf("%d ", t->token.type);
        inorder(t->right);
    }
}

struct ast *constructTree(struct token postfix[])
{
    struct ast *parent;
    struct ast *right;
    struct ast *left;
    int i = 0;

    while (postfix[i].type)
    {
        if (!isParent(postfix[i].type))
            parent = create_node(postfix[i]);
        else
        {
            parent = create_node(postfix[i]);
            right = pop_node();
            left = pop_node();
            parent->left = left;
            parent->right = right;
        }
        push_node(parent);
        i++;
    }
    parent = pop_node();
    inorder(parent);
    return parent;
}

int evaluate(struct ast* ast)
{
    switch (ast->token.type)
    {
        case OR:
            return evaluate(ast->left) || evaluate(ast->right);
        case AND:
            return evaluate(ast->left) || evaluate(ast->right);
        case NOT:
            return !(evaluate(ast->left) && evaluate(ast->right));
        default:
            for (int i = 0; i < FUN_LENGTH; i++)
            {
                if (expressions[i].type == ast->token.type)
                {
                    return expressions[i].function(ast->token.value[0], 0);
                }
            }
        break;
    }

    return 1;
}

int is_newer(char *path, unsigned int *timestamp)
{
    struct stat statbuff;
    lstat(path, &statbuff);
    struct timespec time = statbuff.st_mtim;
    __syscall_slong_t tmpstamp = *timestamp;

    return time.tv_nsec > tmpstamp;
}

int print(char *path, unsigned int *isFolder)
{
    if (isFolder)
        printf("%s\n", path);
    else
        printf("%s\n", path);

    return 1;
}

// In myfind use https://linux.die.net/man/3/getgrnam to get given gid of group
int group_own(char *path, unsigned int *gid)
{
   struct stat statbuff;
   lstat(path, &statbuff);
   gid_t pgid = statbuff.st_gid;

   return pgid == *gid;
}

// Use https://pubs.opengroup.org/onlinepubs/7908799/xsh/getpwnam.html to get uid from login
int user_own(char *path, unsigned int *uid)
{
    struct stat statbuff;
    lstat(path, &statbuff);
    gid_t puid = statbuff.st_uid;

    return puid == *uid;
}

int rm(char *path, unsigned int *placeholder)
{
    placeholder = placeholder;
    if (remove(path) == 0)
        return 1;
    return 0;
}

int has_name(char *path, char *name)
{
    char *pattern = malloc((sizeof(path) + 3) * sizeof(char));
    sprintf(pattern, "*%s*", name);
    if (fnmatch(pattern, name, 0) == 0)
        return 1;
    return 0;
}
