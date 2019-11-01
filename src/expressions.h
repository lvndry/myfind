#ifndef EXP_H
#define EXP_H

struct expression
{
    char *name;
    int (*function)(char *path, void* data);
};

int is_newer(char *path, unsigned int *timestamp);
int print(char *path,  unsigned int *isFolder);
int group_own(char *path, unsigned int *gid);
int user_own(char *path, unsigned int *uid);
int rm(char *path, unsigned int *placeholder);

struct expression expressions[5] = {
    {
        .name = "newer",
        .function = is_newer,
    },
    {
        .name = "print",
        .function = print,
    },
    {
        .name = "group",
        .function = group_own,
    },
    {
        .name = "user",
        .function = user_own,
    },
    {
        .name = "delete",
        .function = rm,
    },
};

#endif
