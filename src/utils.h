#ifndef UTILS_H
#define UTILS_H

# define MODE_WXUSR	(S_IWUSR | S_IXUSR)
# define MODE_R		(S_IRUSR | S_IRGRP | S_IROTH)
# define MODE_RW	(S_IWUSR | S_IWGRP | S_IWOTH | MODE_R)
# define MODE_RWX	(S_IXUSR | S_IXGRP | S_IXOTH | MODE_RW)
# define MODE_ALL	(S_ISUID | S_ISGID | S_ISVTX | MODE_RWX)

int islink(const char *path);
int remove_ds(char *path);
int isNumeric(const char *str);
int toOctal(int decimal);
char *create_template(
    char *template,
    char *arg,
    char **ptr,
    char *pathname,
    int isDir
);
int is_valid_name(char *path);
void format_path(char *path);
void getFilename(char *filename, char*path, char *d_name);
char **build_args(char**argv, char **template, char *pathname, int isDir);

#endif