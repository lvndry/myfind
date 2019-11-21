#ifndef ERR_H
#define ERR_H

enum error_type
{
    CMD_MALFORMED,
    UNKN_PRED,
    MISS_ARG,
    UNKN_ARG,
    INV_ARG,
    PATH,
    PAREN,
};

void func_failure(char *message);
void error_exit(enum error_type type, char *error);
void print_error(char *path, char *error);
void exec_error(void);

#endif