#define _DEFAULT_SOURCE
#include <sys/stat.h>

int islink(char *path)
{
    struct stat buff;
    lstat(path, &buff);
    return S_ISLNK(buff.st_mode);
}
