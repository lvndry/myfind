#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
    char *input = "-- {}{}{} {}{} --";
    char *filename = "./foo/bar/bin";

    char *fc = strstr(input, "{}");
    if (fc == NULL)
    {
        printf("%s\n", input);
        return *input;
    }

    char *target = malloc(sizeof(fc - input) + 1);
    strncpy(target, input, fc - input);
    target[fc - input] = '\0';
    return 0;

    int n = 0;

    char *ptr = input;
    while ((ptr = strstr(ptr, "{}")) != NULL)
    {
        target = realloc(
                 target,
                 sizeof(char) *
                 ((sizeof(target) + sizeof(filename)) + 300)
                );
        strcat(target, filename);
        ptr+= 2;
        n++;
        printf("%s\n", target);
    }
    strcat(target, fc + 2 * n);
    printf("%s\n", target);
    return 0;
}