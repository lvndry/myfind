#include <stdlib.h>

#include "ast.h"
#include "expressions.h"

/* THIS IS PSEUDO CODE. DO NOT COMPILE */

struct node *init(void* expression, enum node_type type)
{
    struct node *ast = malloc(sizeof(struct node));
    ast->right = NULL;
    ast->left = NULL;
    ast->value = expressions;
    ast->type = type;

    return ast;
}

int evaluate(struct node* ast)
{
    switch (ast->type)
    {
    case OR:
        return evaluate(ast->left) || evaluate(ast->right);
    case AND:
        return evaluate(ast->left) || evaluate(ast->right);
    case NOT:
        return !(evaluate(ast->left) && evaluate(ast->right));
    default:
        for (int i = 0; i < FUN_LENGTH; i++)
        {
            if (expressions[i].type == ast->type)
            {
                return expressions[i].function(ast->value, 0);
            }
        }
    }
}
