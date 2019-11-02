#ifndef AST_H
#define AST_H

enum node_type
{
    OR,
    AND,
    NOT,
    PARENTHESES,
    OPERATOR,
    ARG
};

struct token {
    enum node_type type;
    char *value;
};

struct node {
    enum node_type type;
    struct node *left;
    struct node *right;
    char *value;
};

struct node *init(void* value, enum node_type);
void remove_node(struct node *node);
int eval(struct node* expresssion);

#endif
