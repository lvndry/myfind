#ifndef ACTIONEXP_H
#define ACTIONEXP_H

#include "ast.h"

// action functions
int print(struct params *params);
int rm(struct params *params);
int execute(struct params *params);
int executedir(struct params *params);
int executeplus(struct params *params);

#endif