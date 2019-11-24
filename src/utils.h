#ifndef UTILS_H
#define UTILS_H

int islink(const char *path);
int remove_ds(char *path);
int isChmod(const char *str);
char *create_template(
    char *template,
    char *arg,
    char **ptr,
    char *pathname,
    int isDir
);
int is_valid_name(char *path);
void format_path(char *path);
void getFilename(char *filename, char *path, char *d_name);
char **build_args(char **argv, char *pathname, int isDir);
long long octalToDecimal(int octal);

#endif