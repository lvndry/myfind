#define _DEFAULT_SOURCE
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

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
