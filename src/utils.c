#define _DEFAULT_SOURCE
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

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

/*
char *build_exarg(char *dest, char *src, char *filename)
{
    char *template = NULL;

    while ((src = strstr(src, "{}")) != NULL)
    {
        template = realloc(
            template,
            sizeof(char) *
            ((sizeof(src) + sizeof(filename)) + 1000)
        );
        if (template == NULL)
            func_failure("Malloc fail");
        template[0] = 0;

        dest = create_template(
            template,
            src,
            &src,
            filename,
            0
        );
            continue;
    }
}
*/
