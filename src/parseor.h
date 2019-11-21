#ifndef PARSEOR_H
#define PARSEOR_H

#include "parse.h"

// token parsing functions
struct token parse_oparen(char *argv[], int *cursor);
struct token parse_cparen(char *argv[], int *cursor);
struct token parse_or(char *argv[], int *cursor);
struct token parse_and(char *argv[], int *cursor);
struct token parse_not(char *argv[], int *cursor);

#endif