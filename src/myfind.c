#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <getopt.h>

struct opts_t {
    int d;
    int h;
    int l;
    int p;
};

struct opts_t options = { .d = 0, .h = 0, .l = 0, .p = 1 };

int setOptions(int count, char **vector)
{
    for (int i = 1; i < count; i++)
    {
        char *option = vector[i];
        if (option[0] != '-')
            return i;
        if (strcmp(vector[i], "-d") == 0)
            options.l = 1;
        if (strcmp(vector[i], "-L") == 0)
        {
            options.l = 1;
            options.p = 0;
        }
        if (strcmp(vector[i], "-P") == 0)
        {
            options.l = 0;
        }
        if (strcmp(vector[i], "-H") == 0)
            options.h = 1;
    }

    return 0;
}

void getFilename(char *filename, char*path, char *d_name)
{
    sprintf(filename, "%s/%s", path, d_name);
}

int is_valid_name(char *path)
{
    return (strcmp(path, ".") != 0 && strcmp(path, "..") != 0);
}

int ls(char *path)
{  
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        if(access(path, F_OK) != -1)
        {
            printf("%s\n", path);
        }
        else
        {
            // err(1, "%s", path);
            printf("myfind: %s: %s\n", path, strerror(errno));
        }

        return 1;
    }

    struct dirent *file;
    struct stat statbuff;
    char filename[1024];

    file = readdir(dir);
    printf("%s\n", path);

    if (path[strlen(path) - 1] == '/')
        path[strlen(path) - 1] = '\0';

    while ((file = readdir(dir)) != NULL)
    {
        getFilename(filename, path, file->d_name);
        if (is_valid_name(file->d_name))
        {
            if (options.l)
                stat(filename, &statbuff);
            else
                lstat(filename, &statbuff);
            if (S_ISDIR(statbuff.st_mode))
            {
                // printf("%s is a directory\n", filename);
                ls(filename);
            }
            else
            {
                printf("%s\n", filename);
            }
        }
    }
    return closedir(dir);
}

int main(int argc, char **argv)
{
    char *path;
    setOptions(argc, argv);

    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            path = argv[i];
            ls(argv[i]);
        }
    }
    else
    {
        path = ".";
        ls(path);
    }

    return 0;
}
