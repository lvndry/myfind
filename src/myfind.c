#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

int ls(char *path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
        err(1,"%s", path);

    struct dirent *file;
    struct stat statbuff;
    char filename[1024];

    file = readdir(dir);
    printf("%s\n", path);

    while ((file = readdir(dir)) != NULL)
    {
        sprintf(filename, "%s/%s", path, file->d_name);
        if (strcmp(file->d_name, "..") != 0)
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
        path = argv[1];
    else
        path = ".";

    return ls(path);
}
