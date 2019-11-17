#ifndef ERR_H
#define ERR_H

enum error_type
{
    PARSE,
    CMD_MALFORMED
};

void func_failure(char *message);
void parse_error(enum error_type type);

#endif
