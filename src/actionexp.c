#define _DEFAULT_SOURCE
#include <errno.h>
#include <err.h>
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
#include "memory.h"
#include "utils.h"
#include "stack.h"

// actions functions
int print(struct params *params)
{
    printf("%s\n", params->pathname);
    return 1;
}

int rm(struct params *params)
{
    if (remove(params->pathname) == 0)
        return 1;
    fprintf(stderr, "%s\n", strerror(errno));
    return 0;
}

void free_args(char **args)
{
    int i = 0;
    while (args[i] != NULL)
    {
        free(args[i]);
        i++;
    }
    free(args);
    args = NULL;
}

void free_args_plus(char **args, char **execvalue, int nfiles)
{
    free_execargs(execvalue, nfiles);
    free(args);
    execvalue = NULL;
    args = NULL;
}

int exec_child(char **args, char *direcory)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        free_args(args);
        func_failure(strerror(errno));
    }
    else if (pid == 0)
    {
        if (direcory != NULL)
            chdir(direcory);
        if (execvp(args[0], args) == -1)
        {
            free_args(args);
            errx(0, "myfind: %s", strerror(errno));
        }
        free_args(args);
        exit(0);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) == -1)
        {
            free_args(args);
            err(0, "myfind: %s", strerror(errno));
        }
        free_args(args);
        if (WIFEXITED(status))
            return !WEXITSTATUS(status);
        return 0;
    }

    free_args(args);

    return 0;
}

int exec_child_plus(char **args, char **execvalue, int nfiles)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        free_args_plus(args, execvalue, nfiles);
        errx(0, "myfind: %s", strerror(errno));
    }
    else if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            free_args_plus(args, execvalue, nfiles);
            errx(0, "myfind: %s", strerror(errno));
        }
        exit(0);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) == -1)
        {
            free_args_plus(args, execvalue, nfiles);
            func_failure(strerror(errno));
        }
        free_args_plus(args, execvalue, nfiles);
        if (WIFEXITED(status))
            return !WEXITSTATUS(status);
        return 0;
    }

    free_args_plus(args, execvalue, nfiles);

    return 0;
}

int execute(struct params *params)
{
    char **args = build_args(params->argv, params->pathname, 0);
    int res = exec_child(args, NULL);
    return res;
}

int executedir(struct params *params)
{
    char *dup = strdup(params->pathname);
    char *filename = basename(params->filename);
    char *directory = dirname(dup);
    char **args = build_args(params->argv, filename, 1);
    int res = exec_child(args, directory);
    free(dup);
    return res;
}

// Here I suppose that the exec + is correctly parsed and that
// I only have to replace the last {} by the list of files
int executeplus(struct params *params)
{
    static int nfiles = 0;
    static char **execvalue = NULL;

    if (params->pathname == NULL)
    {
        char **args = xmalloc(
            sizeof(char *) *
            (nfiles + sizeof(execvalue) + 100));

        int i = 0;
        for (i = 0; strcmp(params->argv[i], "{}") != 0; i++)
            args[i] = params->argv[i];

        memcpy(args + i, execvalue, sizeof(char *) * nfiles);
        args[i + nfiles] = NULL;

        return exec_child_plus(args, execvalue, nfiles);
    }
    else
    {
        execvalue = xrealloc(execvalue, (sizeof(char *) * (nfiles + 2)));
        execvalue[nfiles] = xmalloc(strlen(params->pathname) + 1);
        strcpy(execvalue[nfiles], params->pathname);
        nfiles += 1;
    }

    return 0;
}