#define _DEFAULT_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast.h"
#include "utils.h"

#define TREE_SIZE 50

int shouldprint = 0;
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
    },
    {
        .type = PERM,
        .function = has_perm,
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
        .type = EXEC,
        .function = execute,
    },
    {
        .type = EXECDIR,
        .function = executedir,
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

void free_ast(struct ast *root)
{
    if (root == NULL)
        return;

    free(root->token.value);
    free_ast(root->left);
    free_ast(root->right);
    free(root);
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
        return 0;

    int len = sizeof(expressions) / sizeof(expressions[0]);
    int i = 0;

    switch (ast->token.type)
    {
        case OR:
            return evaluate(ast->left, pathname, filename) || evaluate(ast->right, pathname, filename);
        case AND:
            return evaluate(ast->left, pathname, filename) && evaluate(ast->right, pathname, filename);
        case NOT:
            return !evaluate(ast->right, pathname, filename);
        default:
            for (i = 0; i < len; i++)
            {
                if (expressions[i].type == ast->token.type)
                {
                    return expressions[i].function(ast->token.value, pathname, filename);
                }
            }
            // TODO: if i == len throw error
        break;
    }

    return 0;
}

int is_newer(char *argv[], char *pathname, char *filenname)
{
    UNUSED(filenname);
    struct stat statbuff;

    stat(argv[0], &statbuff);
    struct timespec timearg = statbuff.st_mtim;
    stat(pathname, &statbuff);
    struct timespec timepath = statbuff.st_mtim;
    shouldprint = 1;
    // TODO: check why tv_nsec does not work
    return timepath.tv_sec > timearg.tv_sec;
}

int print(char *argv[], char *pathname, char *filename)
{
    shouldprint = 0;
    UNUSED(argv);
    UNUSED(filename);

    if (argv[0] != NULL)
        return 1;
    printf("%s\n", pathname);
    return 1;
}

int group_own(char *argv[], char *pathname, char *filename)
{
    shouldprint = 1;
    UNUSED(filename);

    struct group *group = getgrnam(argv[0]);
    if (group == NULL)
        return 0;
    struct stat statbuff;
    stat(pathname, &statbuff);
    return group->gr_gid == statbuff.st_gid;
}

int user_own(char *argv[], char *pathname, char *filename)
{
    shouldprint = 1;
    UNUSED(filename);

    struct passwd *user = getpwnam(argv[0]);
    if (user == NULL)
        return 0;
    struct stat statbuff;
    stat(pathname, &statbuff);
    return user->pw_uid == statbuff.st_uid;
}

int has_name(char *argv[], char *pathname, char *filename)
{
    UNUSED(pathname);
    int offset = remove_ds(filename);

    if (fnmatch(argv[0], filename + offset, FNM_PATHNAME) == 0)
        return 1;
    return 0;
}

int has_type(char *argv[], char *pathname, char *filename)
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
        default: // TODO: Throw error
            fprintf(stderr, "myfind: Invalid type argument");
            exit(EXIT_FAILURE);
    }
    return 1;
}

int has_perm(char *argv[], char *pathname, char *filename)
{
    shouldprint = 1;
    UNUSED(pathname);

    struct stat buf;
    stat(filename, &buf);

    __mode_t statchmod = buf.st_mode & MODE_ALL;

    if (argv[0][0] == '-')
    {
        char *cpy = *argv + 1;
        __mode_t argmode = atoi(cpy);
        int octmod = toOctal(statchmod);
        return (octmod & argmode) == argmode;
    }
    else if (argv[0][0] == '/')
    {
        // Check every bytes and see if a user can do it
        char *cpy = *argv + 1;
        __mode_t argmode = atoi(cpy);
        __mode_t bit1 = cpy[0] - '0';
        __mode_t bit2 = cpy[1] - '0';
        __mode_t bit3 = cpy[2] - '0';

        if (
            (statchmod & S_IRUSR) == bit1
            || (statchmod & S_IRGRP) == bit1
            || ( statchmod & S_IROTH) == bit1
        )
            return 1;
        if (
            (statchmod & S_IWUSR) == bit2
            || (statchmod & S_IWGRP) == bit2
            ||  (statchmod & S_IXGRP) == bit2
        )
            return 1;
        if (
            (statchmod & S_IXUSR) == bit3
            || (statchmod & S_IRGRP) == bit3
            ||  (statchmod & S_IXOTH) == bit3
        )
            return 1;
        return (statchmod & argmode) != 0;
    }
    else if (isNumeric(argv[0]))
    {
        int argmod = atoi(argv[0]);
        int octmod = toOctal(statchmod);
        return octmod == argmod;
    }

    return 0;
    // else error
}

int rm(char *argv[], char *pathname, char *filename)
{
    shouldprint = 0;
    UNUSED(argv);
    UNUSED(filename);


    if (remove(pathname) == 0)
        return 1;
    return 0;
}

int execute(char *argv[], char *pathname, char *filename)
{
    shouldprint = 0;
    UNUSED(filename);

    char *ptr;
    char **args = malloc(100 * sizeof(char));
    int i = 0;
    char *template = NULL;

    for (i = 0; argv[i] != NULL; i++)
    {
        // TODO: Handle several {} in same argv
        if ((ptr = strstr(argv[i], "{}")) != NULL)
        {
            template = malloc(sizeof(char) * (sizeof(argv[i]) + sizeof(pathname)) + 1000);
            template[0] = 0;
            if (template == NULL)
            {
                perror("Malloc fail");
                exit(EXIT_FAILURE);
            }
            args[i] = create_template(template, argv[i], ptr, pathname, 0);
        }
        else
            args[i] = argv[i];
    }

    args[i] = NULL;

    pid_t pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "./myfind: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        execvp(args[0], args);
        return 0;
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        if (template != NULL)
            free(template);
        free(args);

        return !WEXITSTATUS(status);
    }

    if (template != NULL)
        free(template);
    free(args);

    return 0;
}

int executedir(char *argv[], char *pathname, char *filename)
{
    shouldprint = 0;
    UNUSED(pathname);
    char *ptr;
    char **args = malloc(100 * sizeof(char));
    int i = 0;
    char *template = NULL;

    for (i = 0; argv[i] != NULL; i++)
    {
        // TODO: Handle several {} in same argv
        if ((ptr = strstr(argv[i], "{}")) != NULL)
        {
            template = malloc(sizeof(char) * (sizeof(argv[i]) + sizeof(filename)) + 1000);
            template[0] = 0;
            if (template == NULL)
            {
                perror("Malloc fail");
                exit(EXIT_FAILURE);
            }
            args[i] = create_template(template, argv[i], ptr, filename, 1);
        }
        else
            args[i] = argv[i];
    }

    args[i] = NULL;

    pid_t pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "./myfind: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        return execvp(args[0], args);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        if (template != NULL)
            free(template);
        free(args);
        return WEXITSTATUS(status);
    }

    if (template != NULL)
        free(template);
    free(args);

    return 0;
}
