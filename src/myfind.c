#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include<errno.h>

int is_valid_dir(char *path)
{
    return (strcmp(path, "..") != 0 && strcmp(path, ".") != 0);
}

int ls(char *path)
{  
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
       // err(1, "%s", path);
       printf("my find: %s: %s\n", path, strerror(errno));
       return 1;
    }

    struct dirent *file;
    struct stat statbuff;
    char filename[1024];

    file = readdir(dir);
    printf("%s\n", path);

    while ((file = readdir(dir)) != NULL)
    {
        sprintf(filename, "%s/%s", path, file->d_name);
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
        path = argv[1];
    else
        path = ".";

    return ls(path);
}
