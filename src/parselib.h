#ifndef PARSELIB_H
#define PARSELIB_H

#include "parse.h"

// token parsing functions
struct token parse_oparen(char *argv[], int *cursor);
struct token parse_cparen(char *argv[], int *cursor);
struct token parse_or(char *argv[], int *cursor);
struct token parse_and(char *argv[], int *cursor);
struct token parse_not(char *argv[], int *cursor);
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