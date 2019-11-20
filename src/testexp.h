#ifndef TESTEXP_H
#define TESTEXP_H

#include "ast.h"

// test functions
int is_newer(struct params *params);
int group_own(struct params *params);
int user_own(struct params *params);
int has_name(struct params *params);
int has_type(struct params *params);
int has_perm(struct params *params);

#endif