#ifndef MEM_H
#define MEM_H

#include <stdlib.h>

void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
void free_execargs(char **execvalue, int len);

#endif