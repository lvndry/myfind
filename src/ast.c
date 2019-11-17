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
#include "errors.h"

#define TREE_SIZE 50

struct ast *tree[TREE_SIZE] = { NULL };
int treetop = 0;

struct expression expressions[] = {
    // tests
    {
        .type = NAME,
        .function = has_name,
    },
    {
        .type = TYPE,
        .function = has_type,
    },
    {
        .type = NEWER,
        .function = is_newer,
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
    // actions
    {
        .type = PRINT,
        .function = print,
    },
    {
        .type = DELETE,
        .function = rm,
    },
    {
        .type = EXEC,
        .function = execute,
    },
    {
        .type = EXECDIR,
        .function = executedir,
    },
    {
        .type = EXECPLUS,
        .function = executeplus,
    }
};

void push_node(struct ast *node)
{
    tree[treetop++] = node;
}

struct ast *pop_node(void)
{
    return tree[--treetop];
}

struct ast *create_node(struct token token)
{
    struct ast *ast = malloc(sizeof(struct ast));

    ast->token = token;
    ast->right = NULL;
    ast->left = NULL;

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
void inorder(struct ast *node)
{
    if (node)
    {
        inorder(node->left);
        printf("%d ", node->token.type);
        inorder(node->right);
    }
    printf("\n");
}

struct ast *constructTree(struct token postfix[])
{
    struct ast *parent;
    struct ast *right;
    struct ast *left;
    int i = 0;
    int len = 0;

    while (postfix[len].type)
        len++;

    if (len == 0)
        return NULL;

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

int evaluate(struct ast *ast, struct params *params)
{
    if (ast == NULL)
    {
        params->shouldprint = 1;
        return 1;
    }

    int len = sizeof(expressions) / sizeof(expressions[0]);
    int i = 0;

    switch (ast->token.type)
    {
        case OR:
            if (evaluate(ast->left, params))
                return 1;
             return evaluate(ast->right, params);
             break;
        case AND:
            if (evaluate(ast->left, params))
                return evaluate(ast->right, params);
            return 0;
            break;
        case NOT:
            return !evaluate(ast->right, params);
            break;
        default:
            for (i = 0; i < len; i++)
            {
                if (ast->token.type == expressions[i].type)
                {
                    if (ast->token.category == ACTION)
                        params->shouldprint = 0;
                    else
                        params->shouldprint = 1;
                    params->value = ast->token.value;
                    return expressions[i].function(params);
                }
            }
            func_failure("./myfind: Invalid expression type");
        break;
    }
    return 1;
}

// Test functions

int print(struct params *params)
{
    printf("%s\n", params->pathname);
    return 1;
}

int is_newer(struct params *params)
{
    struct stat statbuff;

    stat(params->value[0], &statbuff);
    struct timespec timearg = statbuff.st_mtim;
    stat(params->pathname, &statbuff);
    struct timespec timepath = statbuff.st_mtim;

    // TODO: check why tv_nsec does not work
    return timepath.tv_sec > timearg.tv_sec;
}

int group_own(struct params *params)
{
    struct group *group = getgrnam(params->value[0]);
    if (group == NULL)
        return 0;
    struct stat statbuff;
    stat(params->pathname, &statbuff);

    return group->gr_gid == statbuff.st_gid;
}

int user_own(struct params *params)
{
    struct passwd *user = getpwnam(params->value[0]);
    if (user == NULL)
        return 0;
    struct stat statbuff;
    stat(params->pathname, &statbuff);
    return user->pw_uid == statbuff.st_uid;
}

int has_name(struct params *params)
{
    int offset = remove_ds(params->filename);

    // printf("fnmatch: %d - params->filename: %s\n", fnmatch(params->value[0], params->filename + offset, FNM_PATHNAME), params->filename + offset);
    if (fnmatch(params->value[0], params->filename + offset, FNM_PATHNAME) == 0)
        return 1;
    return 0;
}

int has_type(struct params *params)
{
    struct stat statbuff;
    lstat(params->pathname, &statbuff);

    switch (params->value[0][0])
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
            func_failure("./myfind: Invalid type argument");
            break;
    }
    return 1;
}

int has_perm(struct params *params)
{
    struct stat buf;
    stat(params->filename, &buf);

    __mode_t statchmod = buf.st_mode & MODE_ALL;

    if (params->value[0][0] == '-')
    {
        char *cpy = *(params->value) + 1;
        __mode_t argmode = atoi(cpy);
        int octmod = toOctal(statchmod);
        return (octmod & argmode) == argmode;
    }
    else if (params->value[0][0] == '/')
    {
        // Check every bytes and see if a user can do it
        char *cpy = *(params->value) + 1;
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
    else if (isNumeric(params->value[0]))
    {
        int argmod = atoi(params->value[0]);
        int octmod = toOctal(statchmod);
        return octmod == argmod;
    }

    return 0;
    // else error
}

// Actions functions

int rm(struct params *params)
{
    if (remove(params->pathname) == 0)
        return 1;
    return 0;
}

int execute(struct params *params)
{
    char *ptr;
    char **args = malloc(sizeof(char) * 100);
    int i = 0;
    char *template = NULL;

    for (i = 0; params->value[i] != NULL; ++i)
    {
        // TODO: Handle several {} in same params->value
        if ((ptr = strstr(params->value[i], "{}")) != NULL)
        {
            template = malloc(
                sizeof(char) *
                (sizeof(params->value[i]) + sizeof(params->pathname)) + 1000
            );
            template[0] = 0;
            if (template == NULL)
            {
                perror("Malloc fail");
                exit(EXIT_FAILURE);
            }
            args[i] = create_template(
                template,
                params->value[i],
                ptr,
                params->pathname,
                0
            );
        }
        else
            args[i] = params->value[i];
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

int executedir(struct params *params)
{
    char *ptr;
    char **args = malloc(sizeof(char) * 100);
    int i = 0;
    char *template = NULL;

    for (i = 0; params->value[i] != NULL; ++i)
    {
        // TODO: Handle several {} in same params->value
        if ((ptr = strstr(params->value[i], "{}")) != NULL)
        {
            template = malloc(
                sizeof(char) *
                ((sizeof(params->value[i]) + sizeof(params->filename)) + 1000)
            );
            template[0] = 0;

            if (template == NULL)
                func_failure("Malloc fail");

            args[i] = create_template(
                template,
                params->value[i],
                ptr,
                params->filename,
                1
            );
        }
        else
            args[i] = params->value[i];
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
        return WEXITSTATUS(status);
    }

    if (template != NULL)
        free(template);
    free(args);

    return 0;
}

// Here I suppose that the exec + is correctly parsed and that
// I only have to replace the last {} by the list of files
int executeplus(struct params *params)
{
    static size_t sizelen = 0;
    static int fileslen = 0;

    if (params->filename == NULL)
    {
        // exec all
        char **args = malloc(sizeof(char *)
        * (100 + sizeof(params->execvalue) + fileslen));
        int i = 0;

        while (strcmp(params->value[i], "{}") != 0)
        {
            args[i] = params->value[i];
            i++;
        }

        memcpy(args + i, params->execvalue, sizelen);

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
            if (params->execvalue != NULL)
                free(params->execvalue);
            free(args);
            params->execvalue = NULL;
            if (WIFEXITED(status) == 0)
                return WEXITSTATUS(status);
            return 0;
        }

        if (params->execvalue != NULL)
            free(params->execvalue);
        free(args);
    }
    else
    {
        params->execvalue = realloc(
            params->execvalue,
            (sizeof(char *) * sizelen) + (sizeof(char *) * strlen(params->pathname) + 1)
        );
        params->execvalue[fileslen] = malloc(sizeof(char) * strlen(params->pathname) + 1);
        params->execvalue[fileslen++] = params->pathname;
        sizelen += strlen(params->pathname) + 1;
    }

    return 0;
}
