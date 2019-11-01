enum operand
{
    OR,
    AND,
    NOT,
    PARENTHESES,
};

struct token {
    enum operand type;
    char *value;
};

struct node {
    enum operand operand;
    struct node *left;
    struct node *right;
    char *value;
};

struct node *new_node(char *expression)
