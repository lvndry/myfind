#define _DEFAULT_SOURCE
#include <sys/stat.h>
#include <string.h>

int islink(char *path)
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
