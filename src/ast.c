#define _DEFAULT_SOURCE
#include <fcntl.h>
#include <fnmatch.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "ast.h"
#include "utils.h"

#define TREE_SIZE 50

struct ast *tree[TREE_SIZE] = { NULL };
int treetop = 0;

struct expression expressions[] = {
    {
        .type = NAME,
        .function = has_name,
    },
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
    {
        .type = TYPE,
        .function = has_type,
    }
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
    // inorder(parent);
    return parent;
}

int evaluate(struct ast* ast, char *pathname, char *filename)
{
    if (ast == NULL)
        return 1;

    int len = sizeof(expressions) / sizeof(expressions[0]);
    switch (ast->token.type)
    {
        case OR:
            return evaluate(ast->left, pathname, filename) || evaluate(ast->right, pathname, filename);
        case AND:
            return evaluate(ast->left, pathname, filename) || evaluate(ast->right, pathname, filename);
        case NOT:
            return !(evaluate(ast->left, pathname, filename) && evaluate(ast->right, pathname, filename));
        default:
            for (int i = 0; i < len; i++)
            {
                if (expressions[i].type == ast->token.type)
                {
                    return expressions[i].function(ast->token.value, pathname, filename);
                }
            }
        break;
    }

    return 1;
}

int is_newer(char *argv[], char *timestamp, const char *filenname)
{
    UNUSED(filenname);
    struct stat statbuff;
    lstat(argv[0], &statbuff);
    struct timespec time = statbuff.st_mtim;
    __syscall_slong_t tmpstamp = *timestamp;

    return time.tv_nsec > tmpstamp;
}

int print(char *argv[], char *isFolder, const char *filename)
{
    UNUSED(filename);

    if (isFolder)
        printf("%s\n", argv[0]);
    else
        printf("%s\n", argv[0]);

    return 0;
}

// In myfind use https://linux.die.net/man/3/getgrnam to get given gid of group
int group_own(char *argv[], char *gid, const char *filename)
{
    UNUSED(gid);
    UNUSED(filename);

    struct stat statbuff;
    lstat(argv[0], &statbuff);

   return 1;
}

// Use https://pubs.opengroup.org/onlinepubs/7908799/xsh/getpwnam.html to get uid from login
int user_own(char *argv[], char *uid, const char *filename)
{
    UNUSED(uid);
    UNUSED(filename);

    struct stat statbuff;
    lstat(argv[0], &statbuff);

    return 1;
}

int rm(char *argv[], char *pathname, const char *filename)
{
    if (has_name(argv, pathname, filename) && remove(pathname) == 0)
        return 1;
    return 0;
}

// Should to add const char *
int has_name(char *argv[], char *pathname, const char *filename)
{
    UNUSED(pathname);
    int offset = remove_ds(filename);

    if (fnmatch(argv[0], filename + offset, FNM_PATHNAME) == 0)
        return 1;
    return 0;
}

int has_type(char *argv[], char *pathname, const char *filename)
{
    UNUSED(filename);

    struct stat statbuff;
    lstat(pathname, &statbuff);

    switch (argv[0][0])
    {
        case 'b':
            return S_ISBLK(statbuff.st_mode);
        case 'c':
            return S_ISCHR(statbuff.st_mode);
        case 'd':
            return S_ISDIR(statbuff.st_mode);
        case 'f':
            return S_ISREG(statbuff.st_mode);
        case 'l':
            return S_ISLNK(statbuff.st_mode);
        case 'p':
            return S_ISFIFO(statbuff.st_mode);
        case 's':
            return S_ISSOCK(statbuff.st_mode);
        default:
            fprintf(stderr, "myfind: Invalid type argument");
            exit(EXIT_FAILURE);
    }
    return 1;
}
