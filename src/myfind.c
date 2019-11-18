#define _DEFAULT_SOURCE
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ast.h"
#include "errors.h"
#include "parse.h"
#include "stack.h"
#include "utils.h"

struct opts_t
{
    int l;
    int d;
    int h;
    int p;
};

int setOptions(struct opts_t *options, char *argv[], int start, int end);
int getPaths(char *argv[], int start, int argc);
void getFilename(char *filename, char*path, char *d_name);
void getStat(char *filename, struct stat *statbuff, struct opts_t options);
void setparams(
    struct params *params,
    char *pathname,
    char *filename
);
void print_evaluate(struct ast *ast, char *pathname, char *filename);
int ls(char *path, struct ast *ast, struct opts_t options);
void myfind(char *path, struct ast *ast, struct opts_t options);

int setOptions(struct opts_t *options, char *argv[], int start, int end)
{
    for (int i = start; i < end; i++)
    {
        char *option = argv[i];
        if (option[0] != '-')
            return i - 1;
        if (strcmp(argv[i], "-d") == 0)
        {
            options->l = 0;
            options->d = 1;
            options->h = 0;
            options->p = 0;
        }
        else if (strcmp(argv[i], "-L") == 0)
        {
            options->l = 1;
            options->d = 0;
            options->p = 0;
            options->h = 0;
        }
        else if (strcmp(argv[i], "-P") == 0)
        {
            options->l = 0;
            options->d = 0;
            options->h = 0;
            options->p = 1;
        }
        else if (strcmp(argv[i], "-H") == 0)
        {
            options->l = 0;
            options->d = 0;
            options->h = 1;
            options->p = 0;
        }
        else
            return i - 1;
    }

    return end - 1;
}

int getPaths(char *argv[], int start, int argc)
{
    for (int i = start; i < argc; i++)
    {
        if (argv[i][0] == '-' || argv[i][0] == '!' || argv[i][0] == '(')
            return i - 1;
    }
    return argc - 1;
}

void getFilename(char *filename, char*path, char *d_name)
{
    if (strcmp(path, "/") == 0)
        sprintf(filename, "%s%s", path, d_name);
    else
        sprintf(filename, "%s/%s", path, d_name);
}

void getStat(char *filename, struct stat *statbuff, struct opts_t options)
{
    if (options.l)
        stat(filename, statbuff);
    else
        lstat(filename, statbuff);
}

void setparams(
    struct params *params,
    char *pathname,
    char *filename
)
{
    params->pathname = pathname;
    params->filename = filename;
    params->shouldprint = 0;
}

void print_evaluate(struct ast *ast, char *pathname, char *filename)
{
    if (ast == NULL)
    {
         printf("%s\n", pathname);
         return;
    }

    // params->argv is set in evaluate function
    struct params params = { pathname, filename, NULL, NULL, 0 };
    setparams(&params, pathname, filename);
    int res = evaluate(ast, &params);
    if (res == 1 && params.shouldprint == 1)
        printf("%s\n", pathname);
}

int ls(char *path, struct ast *ast, struct opts_t options)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        if(access(path, R_OK) == -1)
        {
            if (!options.d)
                print_evaluate(ast, path, path);
            print_error(path, strerror(errno));
            if (options.d)
                print_evaluate(ast, path, path);
            return 1;
        }
        else
            print_evaluate(ast, path, path);
        return 0;
    }

    struct dirent *file;
    char filename[PATH_MAX];

    if (!options.d)
        print_evaluate(ast, path, path);

    format_path(path);

    file = readdir(dir);
    if (file == NULL)
    {
        print_evaluate(ast, path, path);
        return 1;
    }

    do
    {
        char *dname = file->d_name;
        getFilename(filename, path, dname);
        if (is_valid_name(dname))
        {
            struct stat statbuff;
            getStat(filename, &statbuff, options);
            if (S_ISDIR(statbuff.st_mode))
            {
                if (
                    !options.d
                    && strstr(filename, dname) == NULL
                )
                    print_evaluate(ast, filename, dname);
                ls(filename, ast, options);
            }
            else
            {
                print_evaluate(ast, filename, dname);
            }
        }
    } while ((file = readdir(dir)) != NULL);

    if (options.d)
        print_evaluate(ast, path, path);

    return closedir(dir);
}

void myfind(char *path, struct ast *ast, struct opts_t options)
{
    if (islink(path) && options.p)
        printf("%s\n", path);
    else
    {
        ls(path, ast, options);
    }
}

int main(int argc, char **argv)
{
    char *path;
    struct opts_t options = { .d = 0, .h = 0, .l = 0, .p = 1 };
    int optend = setOptions(&options, argv, 1, argc);
    int pathend = getPaths(argv, optend + 1, argc);
    struct stack *tokens = parse(argv, pathend + 1, argc);
    struct ast *ast = NULL;
    if (argc > pathend + 1)
        ast = constructTree(tokens);

    if (pathend - optend > 0)
    {
        for (int i = optend + 1; i <= pathend; i++)
        {
            path = argv[i];
            myfind(path, ast, options);
        }
    }
    else
    {
        path = ".";
        myfind(path, ast, options);
    }

    free_ast(ast);
    return 0;
}
