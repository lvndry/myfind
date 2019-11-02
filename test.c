#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    char cmd[] = "find . -name foo -a -print";
    char *operators[] = { "-o", "-a" };
    char *delims = "!()";

    return 0;
}
