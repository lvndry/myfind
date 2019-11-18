#ifndef ERR_H
#define ERR_H

enum error_type
{
    PARSE,
    CMD_MALFORMED,
    UNKN_PRED,
    MISS_ARG
};

void func_failure(char *message);
void parse_error(enum error_type type, char *error);
void print_error(char *path, char *error);

#endif
