#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Each call to concat should lead to a free()
char *concat(int argc, char **argv)
{
    int len = 0;
    char out = malloc(len);

    for (int i = 0; i < argc; i++)
    {
        out = realloc(out, len + strlen(argv[i]));
        strcat(out, argv[i]);
        strcat(out, " ");
    }

    return out;
}
