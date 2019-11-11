#include <stdlib.h>
#include <string.h>

#include "parse.h"

struct bintree *init(void)
{
    struct bintree *tree = malloc(sizeof(struct bintree));
    tree->left = NULL;
    tree->right = NULL;

    return tree;
}

struct bintree *insert(char *exp)
{
    struct bintree *tree = malloc(sizeof(struct bintree));
    tree->value = exp;
    tree->left = NULL;
    tree->right = NULL;

    return tree;
}

void remove(struct bintree *tree)
{
    free(tree);
    tree = NULL;
}

int isOperand(char *operand)
{
    for (int i = 0; operands_table[i].value != NULL; i++)
    {
        if (strcmp(operand, operands_table[i].value) == 0)
            return 1;
    }
    return 0;
}

int parse_or(char *argv[])
{
    return argv[0][0];
}

int parse_and(char *argv[])
{
    return argv[0][0];
}

int parse_openp(char *argv[])
{
    return argv[0][0];
}

int parse_closep(char *argv[])
{
    return argv[0][0];
}

int parse_exclam(char *argv[])
{
    return argv[0][0];
}
