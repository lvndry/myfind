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
        execvp(args[0], args);
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
        return 0;
    }

    if (template != NULL)
        free(template);
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
    char *directory = dirname(dup);
    char **args = build_args(params->argv, &template, params->filename, 1);
    int res = exec_child(args, template, directory);
    free(dup);
    return res;
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