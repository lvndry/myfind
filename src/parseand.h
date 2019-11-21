#ifndef PARSEAND_H
#define PARSEAND_H

#include "parse.h"

struct token parse_name(char *argv[], int *cursor);
struct token parse_type(char *argv[], int *cursor);
struct token parse_newer(char *argv[], int *cursor);
struct token parse_perm(char *argv[], int *cursor);
struct token parse_group(char *argv[], int *cursor);
struct token parse_user(char *argv[], int *cursor);
struct token parse_print(char *argv[], int *cursor);
struct token parse_delete(char *argv[], int *cursor);
struct token parse_exec(char *argv[], int *cursor);
struct token parse_execdir(char *argv[], int *cursor);

#endif