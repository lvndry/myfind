#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "actionexp.h"
#include "ast.h"
#include "errors.h"
#include "memory.h"
#include "stack.h"
#include "testexp.h"
#include "utils.h"

struct expression expressions[] =
{
    // tests
    {
        .type = NAME,
        .function = has_name,
    },
    {
        .type = TYPE,
        .function = has_type,
    },
    {
        .type = NEWER,
        .function = is_newer,
    },
    {
        .type = PERM,
        .function = has_perm,
    },
    {
        .type = GROUP,
        .function = group_own,
    },
    {
        .type = USER,
        .function = user_own,
    },
    // actions
    {
        .type = PRINT,
        .function = print,
    },
    {
        .type = DELETE,
        .function = rm,
    },
    {
        .type = EXEC,
        .function = execute,
    },
    {
        .type = EXECDIR,
        .function = executedir,
    },
    {
        .type = EXECPLUS,
        .function = executeplus,
    }
};

struct ast *create_node(struct token *token)
{
    struct ast *ast = xmalloc(sizeof(struct ast));

    ast->token = token;
    ast->right = NULL;
    ast->left = NULL;

    return ast;
}

void free_ast(struct ast *root)
{
    if (root == NULL)
        return;

    free_ast(root->left);
    free_ast(root->right);
    free(root->token->value);
    free(root->token);
    free(root);
    root = NULL;
}

int isParent(enum token_type type)
{
    if (
        type == OR
        || type == AND
        || type == NOT
        || type == NONE
    )
        return 1;
    return 0;
}

struct ast *create_parent(struct stack_ast *astack, struct token *token)
{
    struct ast *parent = create_node(token);
    struct ast *right = pop_astack(astack);
    struct ast *left = pop_astack(astack);
    parent->left = left;
    parent->right = right;

    return parent;
}

struct ast *constructTree(struct stack *postfix)
{
    if (postfix == NULL)
        return NULL;

    struct ast *parent;
    struct stack_ast *astack = create_astack();
    int i = 0;

    while (postfix->array[i])
    {
        if (!isParent(postfix->array[i]->type))
            parent = create_node(postfix->array[i]);
        else
            parent = create_parent(astack, postfix->array[i]);

        push_astack(astack, parent);
        i++;
    }

    astack->array[astack->size] = NULL;
    parent = pop_astack(astack);

    free_stack(postfix);
    free_astack(astack);

    return parent;
}

int evaluate(struct ast *ast, struct params *params)
{
    if (ast == NULL)
        return 0;

    size_t len = sizeof(expressions) / sizeof(expressions[0]);

    switch (ast->token->type)
    {
    case OR:
        return evaluate(ast->left, params) || evaluate(ast->right, params);
        break;
    case AND:
        return evaluate(ast->left, params) && evaluate(ast->right, params);
        break;
    case NOT:
        return !(evaluate(ast->right, params));
        break;
    default:
        for (size_t i = 0; i < len; i++)
        {
            if (ast->token->type == expressions[i].type)
            {
                if (params->pathname == NULL && ast->token->type != EXECPLUS)
                    return 1;
                params->shouldprint = !(ast->token->category == ACTION);
                params->argv = ast->token->value;
                return expressions[i].function(params);
            }
        }
        free_ast(ast);
        ast = NULL;
        func_failure("./myfind Invalid expression");
    break;
    }
    return 0;
}