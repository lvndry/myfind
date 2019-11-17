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

#include "parse.h"
#include "ast.h"
#include "utils.h"

struct opts_t
{
    int l;
    int d;
    int h;
    int p;
};

struct opts_t options = { .d = 0, .h = 0, .l = 0, .p = 1 };
struct stat statbuff;

int setOptions(int start, int end, char **vector)
{
    for (int i = start; i < end; i++)
    {
        char *option = vector[i];
        if (option[0] != '-')
            return i - 1;
        if (strcmp(vector[i], "-d") == 0)
        {
            options.l = 0;
            options.d = 1;
            options.h = 0;
            options.p = 0;
        }
        else if (strcmp(vector[i], "-L") == 0)
        {
            options.l = 1;
            options.d = 0;
            options.p = 0;
            options.h = 0;
        }
        else if (strcmp(vector[i], "-P") == 0)
        {
            options.l = 0;
            options.d = 0;
            options.h = 0;
            options.p = 1;
        }
        else if (strcmp(vector[i], "-H") == 0)
        {
            options.l = 0;
            options.d = 0;
            options.h = 1;
            options.p = 0;
        }
        else
            return i - 1;
    }

    return end - 1;
}

int getPaths(int start, int argc, char **argv)
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

void getStat(char *filename)
{
    if (options.l)
        stat(filename, &statbuff);
    else
        lstat(filename, &statbuff);
}

int is_valid_name(char *path)
{
    return (strcmp(path, ".") != 0 && strcmp(path, "..") != 0);
}

void format_path(char *path)
{
    if (path[strlen(path) - 1] == '/')
        path[strlen(path) - 1] = '\0';
}

int shouldprint = 0;
void print_evaluate(struct ast *ast, char *pathname, char *filenmae)
{
    evaluate(ast, pathname, filenmae);
    if (shouldprint == 1)
        printf("%s\n", pathname);
}

int ls(char *path, struct ast *ast)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        if(access(path, R_OK) == -1)
        {
            fprintf(stderr, "myfind: %s: %s\n", path, strerror(errno));
            if (options.d)
                print_evaluate(ast, path, path);
            return 1;
        }
        if (options.d)
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
            getStat(filename);
            if (S_ISDIR(statbuff.st_mode))
            {
                if (!options.d && strstr(filename, dname) == NULL)
                    print_evaluate(ast, filename, dname);
                ls(filename, ast);
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

void myfind(char *path, struct ast *ast)
{
    if (islink(path) && options.p)
        printf("%s\n", path);
    else
        ls(path, ast);
}

int main(int argc, char **argv)
{
    char *path;
    int optend = setOptions(1, argc, argv);
    int pathend = getPaths(optend + 1, argc, argv);
    struct token *tokens = parse(argv, pathend + 1, argc);
    struct ast *ast = constructTree(tokens);

    if (pathend - optend > 0)
    {
        for (int i = optend + 1; i <= pathend; i++)
        {
            path = argv[i];
            myfind(path, ast);
        }
    }
    else
    {
        path = ".";
        myfind(path, ast);
    }

    free_ast(ast);
    return 0;
}
