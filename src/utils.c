#define _DEFAULT_SOURCE
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <stdio.h>

int islink(const char *path)
{
    struct stat buff;
    lstat(path, &buff);
    return S_ISLNK(buff.st_mode);
}

int remove_ds(const char *path)
{
    if (strstr(path, "./") == path)
        return 2;
    return 0;
}

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

char *create_template(
    char *template,
    char *arg,
    char *ptr,
    char *pathname,
    int isDir
)
{
    strncat(template, arg, ptr - arg);
    template[ptr - arg] = '\0';

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

    ptr += 2;
    strcat(template, ptr);

    return template;
}
