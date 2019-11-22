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
#include "memory.h"
#include "utils.h"
#include "stack.h"

// actions functions
int rm(struct params *params)
{
    if (remove(params->pathname) == 0)
        return 1;
    return 0;
}

int exec_child(char **args, char *template, char *direcory)
{
    pid_t pid = fork();
    if (pid == -1)
        error_exit(-1, strerror(errno));
    else if (pid == 0)
    {
        if (direcory != NULL)
            chdir(direcory);
        if (execvp(args[0], args) == -1)
            error_exit(-1, strerror(errno));
        exit(errno);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        if (template != NULL)
            free(template);
        free(args);
        if (WIFEXITED(status))
            return !WEXITSTATUS(status);
        print_error("execvp", strerror(errno));
        return status;
    }

    if (template != NULL)
        free(template);
    free(args);

    return 0;
}

int exec_child_plus(char **args, char **execvalue, int nfiles)
{
    pid_t pid = fork();
    if (pid == -1)
        error_exit(-1, strerror(errno));
    else if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
            error_exit(-1, strerror(errno));
        exit(errno);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        free_execargs(execvalue, nfiles);
        execvalue = NULL;
        free(args);
        if (WIFEXITED(status) == 0)
            return WEXITSTATUS(status);
        return 0;
    }

    free_execargs(execvalue, nfiles);
    execvalue = NULL;
    free(args);

    return 0;
}

int execute(struct params *params)
{
    char *template = NULL;
    char **args = build_args(params->argv, &template, params->pathname, 0);
    return exec_child(args, template, NULL);
}

int executedir(struct params *params)
{
    char *template = NULL;
    char *dup = strdup(params->pathname);
    char *filename = basename(params->filename);
    char *directory = dirname(dup);
    char **args = build_args(params->argv, &template, filename, 1);
    int res = exec_child(args, template, directory);
    free(dup);
    return res;
}

void append_path(char ***execvalue, char *pathname, int *nfiles)
{
    execvalue[0] = xrealloc(execvalue, (sizeof(char *) * (*nfiles + 2)));
    execvalue[0][*nfiles] = xmalloc(strlen(pathname) + 1);
    strcpy(execvalue[0][*nfiles], pathname);
    *nfiles += 1;
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
        for (i = 0; strcmp(params->argv[i], "{}") != 0; ++i)
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