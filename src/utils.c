#define _DEFAULT_SOURCE
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>

#include "errors.h"
#include "memory.h"

// file manipulation
int islink(const char *path)
{
    struct stat buff;
    lstat(path, &buff);
    return S_ISLNK(buff.st_mode);
}

int remove_ds(char *path)
{
    while (path[strlen(path) - 1] == '/')
        path[strlen(path) - 1] = '\0';

    if (strstr(path, "./") == path)
        return 2;
    return 0;
}

// numerical
int isChmod(const char *str)
{
    size_t len = strlen(str);

    if (str == NULL || len == 0 || len > 4 || *str == '\0' || isspace(*str))
      return 0;

    for (size_t i = 0; i < len; i++)
    {
        if(str[i] < '0' || str[i] > '7')
            return 0;
    }

    return 1;
}

// string
int is_valid_name(char *path)
{
    return (strcmp(path, ".") != 0 && strcmp(path, "..") != 0);
}

void format_path(char *path)
{
    if (path[strlen(path) - 1] == '/')
        path[strlen(path) - 1] = '\0';
}

void getFilename(char *filename, char *path, char *d_name)
{
    if (strcmp(path, "/") == 0)
        sprintf(filename, "%s%s", path, d_name);
    else
        sprintf(filename, "%s/%s", path, d_name);
}

// args have to be freed at the end of function call
char **build_args(char **argv, char *pathname, int exdir)
{
    int i = 0;
    char *template = NULL;
    char **args = xmalloc(sizeof(char *) * 2000);

    for (i = 0; argv[i] != NULL; i++)
    {
        char *arg = argv[i];
        char *fc = strstr(arg, "{}");
        if (fc == NULL)
        {
            args[i] = strdup(arg);
            continue;
        }

        template = xmalloc(sizeof(fc - arg) + 1);
        strncpy(template, arg, fc - arg);
        template[fc - arg] = '\0';

        int n = 0;
        while ((arg = strstr(arg, "{}")) != NULL)
        {
            template = xrealloc(template,
                sizeof(char) * (strlen(template) + strlen(pathname) + 5));

            if (exdir)
                strcat(template, "./");

            strcat(template, pathname);
            arg += 2;
            n += 1;
        }

        strcat(template, fc + 2 * n);
        args[i] = strdup(template);
        free(template);
    }

     args[i] = NULL;
     return args;
}

long long octalToDecimal(int octal)
{
    int dec = 0;
    int i = 0;

    while (octal != 0)
    {
        dec += (octal % 10) * pow(8, i);
        ++i;
        octal /= 10;
    }

    return dec;
}