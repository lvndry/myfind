#define _DEFAULT_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <grp.h>
#include <libgen.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast.h"
#include "errors.h"
#include "utils.h"
#include "stack.h"

// test functions
int is_newer(struct params *params)
{
    struct stat statbuff;

    if(lstat(params->pathname, &statbuff) == -1)
        error_exit(-1, strerror(errno));
    struct timespec timepath = statbuff.st_mtim;
    if (lstat(params->argv[0], &statbuff) == -1)
        error_exit(-1, strerror(errno));
    struct timespec timearg = statbuff.st_mtim;

    if (timepath.tv_sec == timearg.tv_sec)
        return timepath.tv_nsec > timearg.tv_nsec;
    return timepath.tv_sec > timearg.tv_sec;
}

int group_own(struct params *params)
{
    struct group *group = getgrnam(params->argv[0]);
    if (group == NULL)
    {
        fprintf(
            stderr,
            "myfind: '%s' is not the name of a known group",
            params->argv[0]
        );
        exit(EXIT_FAILURE);
    }
    struct stat statbuff;
    if(lstat(params->pathname, &statbuff) == -1)
         error_exit(-1, strerror(errno));
    return group->gr_gid == statbuff.st_gid;
}

int user_own(struct params *params)
{
    struct passwd *user = getpwnam(params->argv[0]);
    if (user == NULL)
    {
        fprintf(
            stderr,
            "myfind: '%s' is not the name of a known user",
            params->argv[0]
        );
        exit(EXIT_FAILURE);
    }

    struct stat statbuff;
    if(lstat(params->pathname, &statbuff) == -1)
        error_exit(-1, strerror(errno));
    return user->pw_uid == statbuff.st_uid;
}

int has_name(struct params *params)
{
    char *dup = strdup(params->filename);
    char *filename = basename(dup);
    int res = 0;

    if (fnmatch(params->argv[0], filename, FNM_PATHNAME) == 0)
        res = 1;
    free(dup);
    return res;
}

int has_type(struct params *params)
{
    struct stat statbuff;
    if(lstat(params->pathname, &statbuff) == -1)
        error_exit(-1, strerror(errno));

    switch (params->argv[0][0])
    {
    case 'b':
        return S_ISBLK(statbuff.st_mode);
    case 'c':
        return S_ISCHR(statbuff.st_mode);
    case 'd':
        return S_ISDIR(statbuff.st_mode);
    case 'f':
        return S_ISREG(statbuff.st_mode);
    case 'l':
        return S_ISLNK(statbuff.st_mode);
    case 'p':
        return S_ISFIFO(statbuff.st_mode);
    case 's':
        return S_ISSOCK(statbuff.st_mode);
    default:
        error_exit(UNKN_ARG, "-type");
        break;
    }
    return 1;
}

int has_perm(struct params *params)
{
    struct stat statbuff;
    if(lstat(params->pathname, &statbuff) == -1)
        error_exit(-1, strerror(errno));

    __mode_t statchmod = statbuff.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
    __mode_t filemode;

    if (params->argv[0][0] == '-')
    {
        sscanf(params->argv[0], "-%o", &filemode);
        return (filemode & statchmod) == filemode;
    }
    else if (params->argv[0][0] == '/')
    {
        sscanf(params->argv[0], "/%o", &filemode);
        return (filemode & statchmod) != 0;
    }
    else if (isChmod(params->argv[0]))
    {
        sscanf(params->argv[0], "%o", &filemode);
        return filemode == statchmod;
    }
    else
    {
        fprintf(stderr, "./myfind: Invalid mode '%s'\n", params->argv[0]);
        exit(EXIT_FAILURE);
    }

    return 0;
}