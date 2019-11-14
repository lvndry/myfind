#define _GNU_SOURCE
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "parse.h"

struct opts_t
{
    int l;
    int d;
    int h;
    int p;
};

struct opts_t options = { .d = 0, .h = 0, .l = 0, .p = 1 };
struct stat statbuff;

int setOptions(int start, int end, char **vector)
{
    for (int i = start; i < end; i++)
    {
        char *option = vector[i];
        if (option[0] != '-')
            return i - 1;
        if (strcmp(vector[i], "-d") == 0)
        {
            options.l = 0;
            options.d = 1;
            options.h = 0;
            options.p = 0;
        }
        else if (strcmp(vector[i], "-L") == 0)
        {
            options.l = 1;
            options.d = 0;
            options.p = 0;
            options.h = 0;
        }
        else if (strcmp(vector[i], "-P") == 0)
        {
            options.l = 0;
            options.d = 0;
            options.h = 0;
            options.p = 1;
        }
        else if (strcmp(vector[i], "-H") == 0)
        {
            options.l = 0;
            options.d = 0;
            options.h = 1;
            options.p = 0;
        }
        else
            return i - 1;
    }

    return end - 1;
}

int getPaths(int start, int argc, char **argv)
{
    for (int i = start; i < argc; i++)
    {
        if (argv[i][0] == '-' || argv[i][0] == '!' || argv[i][0] == '(')
            return i - 1;
    }
    return argc - 1;
}

void getFilename(char *filename, char*path, char *d_name)
{
    if (strcmp(path, "/") == 0)
        sprintf(filename, "%s%s", path, d_name);
    else
        sprintf(filename, "%s/%s", path, d_name);
}

void getStat(char *filename)
{
    if (options.l)
        stat(filename, &statbuff);
    else
        lstat(filename, &statbuff);
}

int is_valid_name(char *path)
{
    return (strcmp(path, ".") != 0 && strcmp(path, "..") != 0);
}

void format_path(char *path)
{
    if (path[strlen(path) - 1] == '/')
        path[strlen(path) - 1] = '\0';
}

int ls(char *path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        printf("%s\n", path);
        if(access(path, R_OK) == -1)
        {
            fprintf(stderr, "myfind: %s: %s\n", path, strerror(errno));
            return 1;
        }
        return 0;
    }

    struct dirent *file;
    char filename[PATH_MAX];

    if (!options.d)
        printf("%s\n", path);

    file = readdir(dir);
    if (file == NULL)
    {
        printf("%s\n", path);
        return 1;
    }
    do
    {
        getFilename(filename, path, file->d_name);
        if (is_valid_name(file->d_name))
        {
            getStat(filename);
            if (S_ISDIR(statbuff.st_mode))
            {
                ls(filename);
            }
            else
            {
                printf("%s\n", filename);
            }
        }
    } while ((file = readdir(dir)) != NULL);

    if (options.d)
        printf("%s\n", path);

    return closedir(dir);
}

int islink(char *path)
{
    struct stat buff;
    lstat(path, &buff);
    return S_ISLNK(buff.st_mode);
}

void myfind(char *path)
{
    if (islink(path))
    {
        if (options.p)
            printf("%s\n", path);
        else
            ls(path);
    }
    else
        ls(path);
}

int main(int argc, char **argv)
{
    char *path;
    int optend = setOptions(1, argc, argv);
    int pathend = getPaths(optend + 1, argc, argv);
    parse(argv, pathend + 1, argc);
    return 1;
    if (pathend - optend > 0)
    {
        for (int i = optend + 1; i <= pathend; i++)
        {
            path = argv[i];
            myfind(path);
        }
    }
    else
    {
        path = ".";
        myfind(path);
    }

    return 0;
}
