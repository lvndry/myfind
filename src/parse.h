#ifndef PRS_H
#define PRS_H

struct bintree
{
    char *value;
    struct bintree *left;
    struct bintree *right;
};

struct operand
{
    char *value;
    int (*function)(char *argv[]);
};

int parse_or(char *argv[]);
int parse_and(char *argv[]);
int parse_openp(char *argv[]);
int parse_closep(char *argv[]);
int parse_exclam(char *argv[]);

struct operand operands_table[] = {
    {"o", parse_or},
    {"a", parse_and},
    {"(", parse_openp},
    {")", parse_closep},
    {"!", parse_exclam},
    {"newer", parse_exclam},
    {"exec", parse_exclam},
    {"print", parse_exclam},
    {0}
};

int isOperand(char *operand);

#endif
