#define _DEFAULT_SOURCE
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <linux/limits.h>

#include <stdio.h>
#include "errors.h"

// File manipulation
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

// Numerical
int isNumeric(const char * str)
{
    if (str == NULL || *str == '\0' || isspace(*str))
      return 0;
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++)
    {
        if(str[i] < '0' || str[i] > '9')
            return 0;
    }

    return 1;
}

int toOctal(int decimal)
{
    int octaltab[10];
    int octalnum = 0;
    int i;
    int j;

    for(i = 0; decimal > 0; i++)
    {
        octaltab[i] = decimal % 8;
        decimal /= 8;
    }

    for(j = i - 1; j >= 0; j--)
        octalnum = octalnum * 10 + octaltab[j];

    return octalnum;
}

// String
char *create_template(
    char *template,
    char *arg,
    char **ptr,
    char *pathname,
    int isDir
)
{
    strncat(template, arg, *ptr - arg);
    template[*ptr - arg] = '\0';

    if (isDir)
    {
        strcat(template, "./");
        char *lsl = strrchr(pathname, '/');
        if (lsl == NULL)
            strcat(template, pathname);
        else
        {
            lsl += 1;
            strcat(template, lsl);
        }
    }
    else
        strcat(template, pathname);

    *ptr += 2;
    strcat(template, *ptr);

    return template;
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

void getFilename(char *filename, char*path, char *d_name)
{
    if (strcmp(path, "/") == 0)
        sprintf(filename, "%s%s", path, d_name);
    else
        sprintf(filename, "%s/%s", path, d_name);
}

// args have to be freed at the end of function call
char **build_args(char **argv, char **template, char *pathname, int exdir)
{
    int i = 0;
    char *arg;
    char **args = malloc(sizeof(char *) * 2000);

    for (i = 0; argv[i] != NULL; ++i)
    {
        arg = argv[i];
        char *fc = strstr(arg, "{}");
        if (fc == NULL)
        {
            args[i] = arg;
            continue;
        }

        *template = malloc(sizeof(fc - arg) + 1);

        strncpy(*template, arg, fc - arg);
        template[0][fc - arg] = '\0';

        int n = 0;
        while ((arg = strstr(arg, "{}")) != NULL)
        {
            *template = realloc(
                *template,
                sizeof(char) *
                (sizeof(*template) + sizeof(pathname) + PATH_MAX)
            );

            if (*template == NULL)
                func_failure("Malloc fail");

            if (exdir)
                strcat(*template, "./");

            strcat(*template, pathname);
            arg += 2;
            n += 1;
        }

        strcat(*template, fc + 2 * n);
        args[i] = *template;
    }

     args[i] = NULL;

     return args;
}