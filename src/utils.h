#ifndef UTILS_H
#define UTILS_H

#define UNUSED(x) (void)(x)

int islink(const char *path);
int remove_ds(const char *path);
int isNumeric(const char *str);

#endif
