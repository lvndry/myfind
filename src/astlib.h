#ifndef ASTLIB_H
#define ASTLIB_H

#include "ast.h"

// evaluation functions
int is_newer(struct params *params);
int print(struct params *params);
int group_own(struct params *params);
int user_own(struct params *params);
int rm(struct params *params);
int has_name(struct params *params);
int has_type(struct params *params);
int has_perm(struct params *params);
int execute(struct params *params);
int executedir(struct params *params);
int executeplus(struct params *params);

#endif