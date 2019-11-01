#ifndef EXP_H
#define EXP_H

struct expression
{
    char *name;
    int (*function)(char *path, int data);
};

int is_newer(char *path, int timestamp);
int print(char *path, int isFolder);
int group_own(char *path, int gid);
int user_own(char *path, int uid);
int rm(char *path, int placeholder);

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
