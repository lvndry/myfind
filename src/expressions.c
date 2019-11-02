#define _GNU_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "expressions.h"

int is_newer(char *path, unsigned int *timestamp)
{
    struct stat statbuff;
    lstat(path, &statbuff);
    struct timespec time = statbuff.st_mtim;

    return time.tv_nsec > timestamp;
}

int print(char *path, unsigned int *isFolder)
{
    printf("%s\n", path);
    return 1;
}

// In myfind use https://linux.die.net/man/3/getgrnam to get given gid of group
int group_own(char *path, unsigned int *gid)
{
   struct stat statbuff;
   lstat(path, &statbuff);
   gid_t pgid = statbuff.st_gid;

   return pgid == gid;
}

// Use https://pubs.opengroup.org/onlinepubs/7908799/xsh/getpwnam.html to get uid from login
int user_own(char *path, unsigned int *uid)
{
    struct stat statbuff;
    lstat(path, &statbuff);
    gid_t puid = statbuff.st_uid;

    return puid == uid;
}

int rm(char *path, unsigned int *placeholder)
{
    if (remove(path) == 0)
        return 1;
    return 0;
}

int has_name(char *path, char *name)
{
    if (strcmp(path, name) == 0)
        return 1;
    return 0;
}

int is_operator(char *operator)
{
    if (
        strcmp(operator, "-o") == 0
        || strcmp(operator, "-a") == 0
        || strcmp(operator, "!") == 0
        || strcmp(operator, "(") == 0
        || strcmp(operator, ")") == 0
    )
    return 1;

    return 0;
}
