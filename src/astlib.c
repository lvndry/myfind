#define _DEFAULT_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <grp.h>
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

int print(struct params *params)
{
    printf("%s\n", params->pathname);
    return 1;
}

// test functions

int is_newer(struct params *params)
{
    struct stat statbuff;

    lstat(params->argv[0], &statbuff);
    struct timespec timearg = statbuff.st_mtim;
    lstat(params->pathname, &statbuff);
    struct timespec timepath = statbuff.st_mtim;

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
    lstat(params->pathname, &statbuff);

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
    lstat(params->pathname, &statbuff);
    return user->pw_uid == statbuff.st_uid;
}

int has_name(struct params *params)
{
    char *filename = malloc(sizeof(char) * (strlen(params->filename) + 1));
    strcpy(filename, params->filename);
    int offset = remove_ds(filename);
    int res = 0;

    if (fnmatch(params->argv[0], filename + offset, FNM_PATHNAME) == 0)
        res = 1;
    free(filename);
    return res;
}

int has_type(struct params *params)
{
    struct stat statbuff;
    lstat(params->pathname, &statbuff);

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
    struct stat buf;
    stat(params->filename, &buf);

    __mode_t statchmod = buf.st_mode & MODE_ALL;

    if (params->argv[0][0] == '-')
    {
        char *cpy = *(params->argv) + 1;
        __mode_t argmode = atoi(cpy);
        int octmod = toOctal(statchmod);
        return (octmod & argmode) == argmode;
    }
    else if (params->argv[0][0] == '/')
    {
        // Check every bytes and see if a user can do it
        char *cpy = *(params->argv) + 1;
        __mode_t argmode = atoi(cpy);
        __mode_t bit1 = cpy[0] - '0';
        __mode_t bit2 = cpy[1] - '0';
        __mode_t bit3 = cpy[2] - '0';

        if (
            (statchmod & S_IRUSR) == bit1
            || (statchmod & S_IRGRP) == bit1
            || ( statchmod & S_IROTH) == bit1
        )
            return 1;
        if (
            (statchmod & S_IWUSR) == bit2
            || (statchmod & S_IWGRP) == bit2
            ||  (statchmod & S_IXGRP) == bit2
        )
            return 1;
        if (
            (statchmod & S_IXUSR) == bit3
            || (statchmod & S_IRGRP) == bit3
            ||  (statchmod & S_IXOTH) == bit3
        )
            return 1;
        return (statchmod & argmode) != 0;
    }
    else if (isNumeric(params->argv[0]))
    {
        int argmod = atoi(params->argv[0]);
        int octmod = toOctal(statchmod);
        return octmod == argmod;
    }

    return 0;
    // else error
}

// actions functions

int rm(struct params *params)
{
    if (remove(params->pathname) == 0)
        return 1;
    return 0;
}

int execute(struct params *params)
{
    char *ptr;
    char **args = malloc(sizeof(char) * 100);
    int i = 0;
    char *template = NULL;

     for (i = 0; params->argv[i] != NULL; ++i)
     {
         ptr = params->argv[i];
         while ((ptr = strstr(ptr, "{}")) != NULL)
         {
             // printf("%s\n", ptr);
             template = realloc(
                 template,
                 sizeof(char) *
                 ((sizeof(args[i]) + sizeof(params->filename)) + 1000)
                );
            if (template == NULL)
                func_failure("Malloc fail");
            template[0] = 0;

            args[i] = create_template(
                template,
                params->argv[i],
                &ptr,
                params->filename,
                0
            );
            continue;
        }
        args[i] = params->argv[i];
    }

    args[i] = NULL;

    pid_t pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "./myfind: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        execvp(args[0], args);
        return 0;
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        if (template != NULL)
            free(template);
        free(args);

        return !WEXITSTATUS(status);
    }

    if (template != NULL)
        free(template);
    free(args);

    return 0;
}

int executedir(struct params *params)
{
    // char *ptr;
    params = params;
    char **args = malloc(sizeof(char) * 100);
    int i = 0;
    char *template = NULL;

    args[i] = NULL;

    pid_t pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "./myfind: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        execvp(args[0], args);
        return 0;
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        if (template != NULL)
            free(template);
        free(args);
        return WEXITSTATUS(status);
    }

    if (template != NULL)
        free(template);
    free(args);

    return 0;
}

// Here I suppose that the exec + is correctly parsed and that
// I only have to replace the last {} by the list of files
int executeplus(struct params *params)
{
    size_t sizelen = 0;
    int fileslen = 0;

    if (params->filename == NULL)
    {
        // exec all
        char **args = malloc(sizeof(char *)
        * (100 + sizeof(params->execvalue) + fileslen));
        int i = 0;

        while (strcmp(params->argv[i], "{}") != 0)
        {
            args[i] = params->argv[i];
            i++;
        }

        memcpy(args + i, params->execvalue, sizelen);

        pid_t pid = fork();
        if (pid == -1)
        {
            fprintf(stderr, "./myfind: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            execvp(args[0], args);
            return 0;
        }
        else
        {
            int status;
            waitpid(pid, &status, 0);
            if (params->execvalue != NULL)
                free(params->execvalue);
            free(args);
            params->execvalue = NULL;
            if (WIFEXITED(status) == 0)
                return WEXITSTATUS(status);
            return 0;
        }

        if (params->execvalue != NULL)
            free(params->execvalue);
        free(args);
    }
    else
    {
        params->execvalue = realloc(
            params->execvalue,
            (sizeof(char *) * sizelen) + (sizeof(char *) * strlen(params->pathname) + 1)
        );
        params->execvalue[fileslen] = malloc(sizeof(char) * strlen(params->pathname) + 1);
        strcpy(params->execvalue[fileslen++], params->pathname);
        sizelen += strlen(params->pathname) + 1;
    }

    return 0;
}