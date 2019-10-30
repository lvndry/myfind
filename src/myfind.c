#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include<errno.h>

void getFilename(char *filename, char*path, char *d_name)
{
    sprintf(filename, "%s/%s", path, d_name);
}

int is_valid_dir(char *path)
{
    return (strcmp(path, ".") != 0 && strcmp(path, "..") != 0);
}

int ls(char *path)
{  
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
       // err(1, "%s", path);
       printf("myfind: %s: %s\n", path, strerror(errno));
       return 1;
    }

    struct dirent *file;
    struct stat statbuff;
    char filename[1024];

    file = readdir(dir);
    printf("%s\n", path);

    if (path[strlen(path) - 1] == '/')
        path[strlen(path) - 1] = 0;

    while ((file = readdir(dir)) != NULL)
    {
        getFilename(filename, path, file->d_name);
        if (is_valid_dir(file->d_name))
        {
            stat(filename, &statbuff);
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
