#include <stdlib.h>

#include "errors.h"

void *xmalloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL)
        func_failure("malloc fail");
    return ptr;
}

void *xrealloc(void *ptr, size_t size)
{
    ptr = realloc(ptr, size);
    if (ptr == NULL)
        func_failure("realloc fail");
    return ptr;
}

void free_execargs(char **execvalue, int len)
{
    if (execvalue == NULL || len == 0)
        return;
    for (int i = 0; i < len; i++)
        free(execvalue[i]);
    free(execvalue);
}